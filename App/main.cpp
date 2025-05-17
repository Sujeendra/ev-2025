#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <QTimeZone>
#include <QObject>
#include <QQmlContext>
#include <QtVirtualKeyboard>
#include <QTimer>
#include <qqml.h>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QVariant>
#include <QCanBus>
#include <QCanDbcFileParser>
#include <QCanBusFrame>
#include <QCanBusDevice>
#include <QRandomGenerator>
#include <cmath>
#include <QtSql/QtSql>
#include <QDebug>
#include "autogen/environment.h"
#include <algorithm>
#include <vector>
#include<gpiod.h>
#include <QProcess>
#include <QNetworkInterface>
#include <thread>
#include "SystemManager.h"
#include "LogManager.h"
#include "WifiManager.h"
#include "Crc32Calculator.h"
#include "DatabaseManager.h"
#include "SignalValueMap.h"
#include "utility.h"
#include "TreeModel.h"
#include "TimeManager.h"
#include "fa.h"
#include <QTextStream>
#include <QHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>


// QFile file("/root/perf_messages_time.csv");
// QTextStream stream(&file);

// Function to convert QByteArray payload to a hex string
// QString toHexString(const QByteArray& data) {
//     return data.toHex(' ').toUpper();
// }

// // Map to store timestamp counters for each CAN ID
// QHash<quint32, quint64> timestampMap;

// // Function to log CAN data into a CSV file
// void logCANData(const QCanBusFrame& frame, QHash<quint32, quint64>& timestampMap, QTextStream& stream) {
//     quint32 canId = frame.frameId();

//     // Increment timestamp for each unique CAN ID
//     timestampMap[canId]++;

//     // Convert data payload to hex string
//     QString dataString = toHexString(frame.payload());

//     // Write to file
//     stream << timestampMap[canId] << ","
//            << QString::number(canId, 16).toUpper() << ","
//            << dataString << "\n";
// }

bool IsConnectForDriving = false;
bool IsDataTransmissionStarted = false;
bool IsConnectedForDCCharging = false;
bool IsConnectedForACCharging=false;
LogManager logManager;
bool SendGPIOSignal = false;
bool HVReq=false;  // High voltage request
bool IsBatteryContactorClosed = false;
std::vector<uint32_t> frameIdsToForward = {0x1918FF71, 0x1919FF71, 0x191AFF71, 0xCFF91FD, 0xCFF92FD}; // both  sevcon hvlp and editron inverter data is forwarded but hvlp one is assumed we have to configure it for three  todo


QMap<QString, double> dataVec;



//   <------------------------------------------------------FSM related UTILS Starts here -------------------------------------------------------------------------->

QJsonObject loadFSMFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open FSM file";
        return {};
    }
    QByteArray data = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << err.errorString();
        return {};
    }
    return doc.object();
}

bool evaluateSingleCondition(const QJsonObject& cond,
                             const QMap<QString, SignalInfo>& signalValueMap) {
    QString key = cond["key"].toString();
    if (!signalValueMap.contains(key)) return false;
    //if not valid then return false as there is no data
    if(!signalValueMap[key].IsValid) return false;

    double actual = signalValueMap[key].value;

    if (cond.contains("eq")) return qFuzzyCompare(actual + 1.0, cond["eq"].toDouble() + 1.0);
    if (cond.contains("ne")) return !qFuzzyCompare(actual + 1.0, cond["ne"].toDouble() + 1.0);
    if (cond.contains("lt")) return actual < cond["lt"].toDouble();
    if (cond.contains("gt")) return actual > cond["gt"].toDouble();
    if (cond.contains("le")) return actual <= cond["le"].toDouble();
    if (cond.contains("ge")) return actual >= cond["ge"].toDouble();
    if (cond.contains("in")) {
        auto jsonArray = cond["in"].toArray();
        for (const auto& v : jsonArray) {
            if (v.toDouble() == actual) {
                return true;
            }
        }
        return false;
    }

    return false;
}

bool evaluateFSMCondition(const QJsonObject& condition,
                          const QMap<QString, SignalInfo>& signalValueMap) {
    if (condition.contains("all")) {
        QJsonArray all = condition["all"].toArray();
        for (const auto& item : all) {
            if (!evaluateSingleCondition(item.toObject(), signalValueMap))
                return false;
        }
        return true;
    }
    if (condition.contains("any")) {
        QJsonArray any = condition["any"].toArray();
        for (const auto& item : any) {
            if (evaluateSingleCondition(item.toObject(), signalValueMap))
                return true;
        }
        return false;
    }

    // fallback for old flat condition style:
    for (auto it = condition.begin(); it != condition.end(); ++it) {
        const QString& key = it.key();
        double expected = it.value().toDouble();
        if (!signalValueMap.contains(key)) return false;
        if(!signalValueMap[key].IsValid) return false;
        if (!qFuzzyCompare(signalValueMap[key].value + 1.0, expected + 1.0)) return false;
    }
    return true;
}

// recheck if the new key created works without explicitly inserting
void performFSMAction(const QJsonObject& action,
                      QMap<QString, double>& dataVec)
{
    QString key = action["signal"].toString().trimmed();
    double value = action["value"].toDouble();
    dataVec[key] = value;
    // the output of this is shown in the application output
    // qDebug() << "[FSM Action] Set" << key << "=" << value;
}

QString runFSMTransition(QJsonObject& fsm,
                         const QString& currentState,
                         const QMap<QString, SignalInfo>& signalValueMap,
                         QMap<QString, double>& dataVec)
{
    QJsonArray states = fsm["states"].toArray();

    for (const QJsonValue& stateVal : states) {
        QJsonObject stateObj = stateVal.toObject();
        if (stateObj["name"].toString() != currentState) continue;

        QJsonArray transitions = stateObj["transitions"].toArray();
        for (const QJsonValue& transVal : transitions) {
            QJsonObject trans = transVal.toObject();
            if (evaluateFSMCondition(trans["condition"].toObject(), signalValueMap)) {
                QJsonArray actions = trans["actions"].toArray();
                for (const QJsonValue& actVal : actions) {
                    performFSMAction(actVal.toObject(), dataVec);
                }
                // logManager.addLog("Next State is: ");
                // logManager.addLog(trans["nextState"].toString());
                return trans["nextState"].toString();
            }
        }
    }

    return currentState; // No transition
}
//    <---------------------------------------------------------------FSM related Utils ends here --------------------------------------------------------------------->

//can 0 is always code as vehicle CAN

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    int rowCount(const QModelIndex & = QModelIndex()) const override
    {
        return 200;
    }

    int columnCount(const QModelIndex & = QModelIndex()) const override
    {
        return 200;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role) {
        case Qt::DisplayRole:
            return QString("%1, %2").arg(index.column()).arg(index.row());
        default:
            break;
        }

        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return { {Qt::DisplayRole, "display"} };
    }
};
// Function to find and return a Message object by matching the first SDM entry's message name
Message* findMessageInSDM(const std::string& messageName, std::unordered_map<std::string, Message>& messages) {
    // Iterate through each message in the map
    for (auto& [pgn, message] : messages) {
        // Check if the SDM vector has entries and if the first entry's message name matches
        if (!message.SDM.empty() && message.SDM[0].message == messageName) {
            return &message;  // Return the matching message object
        }
    }
    // If no match was found, return an first element
    return nullptr;
}

void CreateSDMData(QCanDbcFileParser& fileParser, std::unordered_map<std::string, Message>& messages) {
    std::string target_node = "Vehicle_Control_Unit";

    // Loop over all message descriptions from the parsed DBC file
    const auto descriptions = fileParser.messageDescriptions();
    for (const auto& description : descriptions) {
        if (description.transmitter() == target_node) {
            // Find the matching message in SDM
            Message* result = findMessageInSDM(description.name().toStdString(), messages);
            if (result != nullptr) {

                // Loop over all signal descriptions in the message
                for (const auto& signal : description.signalDescriptions()) {
                    auto value_it = std::find_if((*result).SDM.begin(), (*result).SDM.end(),
                                                 [&signal](const DataEntry& entry) {
                                                     return entry.signal == signal.name().toStdString();
                                                 });

                    // Check if the element was found in SDM
                    if (value_it != (*result).SDM.end()) {
                        QString key = QString::fromStdString(target_node) + "." +
                                      description.name() + "." +
                                      signal.name();
                        // Add check to read value from shared data if it has changed there
                        for (int i = 0; i < dataVec.size(); ++i) {
                            if (dataVec.contains(key)){
                                value_it->value =  dataVec[key];
                                break;
                            }
                        }

                        double value = value_it->value;
                        // Encode the signal value using the QtCanDbc signal encoding method
                        encodeSignal(value, signal.offset(), signal.factor(), signal.startBit(), signal.bitLength(), (*result).sdmSignedData, signal.dataEndian(),signal.dataFormat());
                    }
                }
            }
        }
    }
}
class TimeDateResponder : public QObject {
    Q_OBJECT

public:
    explicit TimeDateResponder( QMap<QString, SignalInfo>& signalValueMap,QJsonObject& fsm)
        : signalValueMap(signalValueMap),fsm(fsm) {}

    void initialize() {
        // Initialize CAN Bus Device
        canDevice = QCanBus::instance()->createDevice("socketcan", "can0");
        if (!canDevice) {
            logManager.addLog("Error: Could not create CAN device.");
            return;
        }

        if (!canDevice->connectDevice()) {
            logManager.addLog("Error: Could not connect to CAN device.");
            return;
        }

        logManager.addLog("CAN 0 device connected successfully.");
        // Initialize CAN1
        canDevice1 = QCanBus::instance()->createDevice("socketcan", "can1");
        if (!canDevice1) {
            logManager.addLog("Error: Could not create CAN1 device.");
            return;
        }
        if (!canDevice1->connectDevice()) {
            logManager.addLog("Error: Could not connect to CAN1.");
            return;
        }
        logManager.addLog("CAN1 connected successfully.");
        currentState = fsm["initialState"].toString();
        qDebug() << "Starting FSM in state:" << currentState;
        // Connect to frame reception
        connect(canDevice, &QCanBusDevice::framesReceived, this, &TimeDateResponder::processFrames);
    }

private:
    QCanBusDevice *canDevice = nullptr;
    QCanBusDevice *canDevice1 = nullptr;
    QMap<QString, SignalInfo>& signalValueMap;
    QJsonObject& fsm;
    QString currentState;


    void processFrames() {


        while (canDevice->framesAvailable()) {
            QCanBusFrame frame = canDevice->readFrame();

            //use this only when you have to log csv data
            // if(IsDataTransmissionStarted)
            // {
            //     logCANData(frame, timestampMap, stream);
            // }

            if (isTimeDateRequest(frame)) {
                // qDebug() << "Received Time/Date request PGN 0x0FEE6";

                // Prepare response frame
                QCanBusFrame responseFrame = createTimeDateResponse();

                // Send response
                canDevice->writeFrame(responseFrame);

            }
            // if(!IsDataTransmissionStarted)
            //     continue;

            QString nextState = runFSMTransition(fsm, currentState, signalValueMap, dataVec);
            if (nextState != currentState) {
                qDebug() << "[FSM] Transitioned:" << currentState << "→" << nextState;
                currentState = nextState;
            }/* else {
                qDebug() << "[FSM] No transition from state:" << currentState;
            }*/
            // frame forward
            if (std::find(frameIdsToForward.begin(), frameIdsToForward.end(), frame.frameId()) != frameIdsToForward.end()) {
                // Forward frame to can1
                canDevice1->writeFrame(frame);
            }

        }
    }

    bool isTimeDateRequest(const QCanBusFrame &frame) {
        return (frame.frameId()) == 0xEA21F3 &&
               frame.payload()[0] == 0xE6 &&
               frame.payload()[1] == 0xFE &&
               frame.payload()[2] == 0x00;
    }

    QCanBusFrame createTimeDateResponse() {
        QCanBusFrame frame;
        frame.setFrameId(0x18FEE621);
        frame.setFrameType(QCanBusFrame::DataFrame);
        QByteArray payload(8, 0);
        // Get the current time in UTC
        QDateTime utcTime = QDateTime::currentDateTimeUtc();

        // Populate UTC time
        payload[0] = utcTime.time().second() / 0.25;
        payload[1] = utcTime.time().minute();
        payload[2] = utcTime.time().hour();
        payload[3] = utcTime.date().month();
        payload[4] = utcTime.date().day() / 0.25;
        payload[5] = utcTime.date().year() - 1985;

        // Set offset to 0
        payload[6] = 125;
        payload[7] = 125;
        frame.setPayload(payload);
        return frame;
    }
};

void printModelData(QSqlQueryModel *model)
{
    // Check if the model is valid
    if (model && model->rowCount() > 0) {
        // Loop through all the rows
        for (int row = 0; row < model->rowCount(); ++row) {
            // Loop through all columns
            QString rowData;
            for (int col = 0; col < model->columnCount(); ++col) {
                rowData += model->data(model->index(row, col)).toString() + " | ";
            }
            qDebug() << rowData; // Print each row
        }
    } else {
        qDebug() << "No data available in the model";
    }
}

void CreateSHMData(std::pair<const std::string, Message> &pair, uint8_t decimalValue)
{
    auto &pgn = pair.first;
    auto &message = pair.second;

    auto hex29bit = message.messageId29Bit;
    hex29bit &= 0x1FFFFFFF;

    // Extract 8 bits from the least significant side (right-hand side)
    uint8_t first8 = hex29bit & 0xFF;         // Extract bits 0-7 source address
    uint8_t second8 = (hex29bit >> 8) & 0xFF; // Extract bits 8-15 pdu-s
    uint8_t third8 = (hex29bit >> 16) & 0xFF; // Extract bits 16-23 pdu-f

    // Negate (bitwise NOT) each 8-bit section
    first8 = ~first8;
    second8 = ~second8;
    third8 = ~third8;


    uint8_t maskedValue = decimalValue & 0x1F; // 0x1F is binary 00011111
    // std::cout<<std::dec<<static_cast<int>(maskedValue)<<std::endl;
    // Shift the masked 5-bit value to the left by 3 to make room for the last 3 bits
    uint8_t result = (maskedValue << 3) | 0x07; // 0x07 is binary 00000111

    uint32_t high = combineBytesTo32Bit(result, first8, second8, third8);

    // Input bytes as specified in the question
    // need to be carefull because the data might have negative data -- need to change the code here
    std::array<uint8_t, 8> inputBytes = message.sdmSignedData; // data to be loaded from excel too specifically formed

    Crc32Calculator calculator;
    calculator.CalculateCrcTable_CRC32();
    uint32_t low = ReverseByteOrder(calculator.Compute_CRC32(inputBytes));

    uint64_t combined = CombineTo64Bit(high, low);
    for (int i = 7; i >= 0; i--)
    {
        message.shmSignedData[i] = static_cast<uint8_t>((combined >> (8 * (7-i))) & 0xFF);
    }
}
void delayedSetGpio(int pin, bool status) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));  // Sleep for 2 seconds
    setGpioPin(pin, status);  // Call the function after sleep
}
class CanMessageSenderGroup1 : public QThread {
    Q_OBJECT
public:
    CanMessageSenderGroup1(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages)
        : fileParser(parser), messages(messages) {}

protected:
    void run() override {
        sendShmSDMMessage(fileParser, messages);
    }

private:
    QCanDbcFileParser& fileParser;
    std::unordered_map<std::string, Message>& messages;

    void sendShmSDMMessage(QCanDbcFileParser& fileParser, std::unordered_map<std::string, Message>& messages) {

        std::unordered_map<std::string, std::chrono::steady_clock::time_point> nextSendTime;
        for (auto& pair : messages) {
            nextSendTime[pair.first] = std::chrono::steady_clock::now();
        }

        QCanBusDevice* canBusDevice = QCanBus::instance()->createDevice("socketcan", "can0");
        canBusDevice->connectDevice();
        bool localGPIODataSentAlreadyfor1 = false;
        bool localGPIODataSentAlreadyfor0 = true;
        while (true) {

            if(!IsDataTransmissionStarted)
            {
                //reset timing
                for (auto& pair : messages) {
                    nextSendTime[pair.first] = std::chrono::steady_clock::now();
                }
                continue;
            }

            // Prepare SHM and SDM data and send them via CAN bus
            for (auto& pair : messages) {

                auto& message = pair.second;
                //if not connected for driving dont send this message
                // if(message.SDM[0].message == "S1_ES_PGN61427" && !IsConnectForDriving && !HVReq)
                // {
                //     nextSendTime[pair.first] = std::chrono::steady_clock::now();
                //     continue;
                // }

                std::string messageId = pair.first;
                // if (message.sheetName != "Main")
                //     continue;

                auto now = std::chrono::steady_clock::now();
                if (now >= nextSendTime[messageId]) {
                    auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(now - nextSendTime[messageId]);
                    // Add the cycleTime to the timeDifference
                    auto totalTime = timeDifference + std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));

                    // stream << totalTime.count() << ","
                    //        <<  QString::number(message.messageId29Bit, 16).toUpper() << "\n";
                    CreateSDMData(fileParser, messages);

                    if(message.sheetName == "Main")
                        CreateSHMData(pair, message.shmCounter);

                    if(message.sheetName == "Main")
                    {
                        std::string target_node = "Vehicle_Control_Unit";
                        for (const auto& msg : fileParser.messageDescriptions()) {
                            if (msg.transmitter() == target_node && message.SHM[0].message == msg.name()) {

                                quint32 shmId = static_cast<quint32>(msg.uniqueId());
                                QByteArray shmDataArray(reinterpret_cast<const char*>(message.shmSignedData.data()), msg.size());
                                QCanBusFrame shmFrame(shmId, shmDataArray);
                                shmFrame.setFrameType(QCanBusFrame::DataFrame);
                                // if(msg.name()!="S1_ES_PGN61427_SHM_Rx")
                                canBusDevice->writeFrame(shmFrame);


                                nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SHM[0].cycleTime));

                                QThread::msleep(static_cast<unsigned long>(message.SDM[0].deltaTime));

                                QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), msg.size()));
                                sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                                canBusDevice->writeFrame(sdmFrame);
                                message.incrementCounter();
                                break;
                            }
                        }
                    }
                    else{
                        // version 2 changes using dlc from the dbc to send only data in the payload
                        std::string target_node = "Vehicle_Control_Unit";
                        for (const auto& msg : fileParser.messageDescriptions()) {
                            if (msg.transmitter() == target_node && message.SDM[0].message == msg.name()) {
                                nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));
                                QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), msg.size()));
                                sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                                canBusDevice->writeFrame(sdmFrame);
                                break;
                            }
                        }


                    }
                }
            }
            //send this after safety signals are sent
            if(!localGPIODataSentAlreadyfor0 && SendGPIOSignal)
            {
                // QCanBusFrame frame;
                // frame.setFrameId(0x1999);
                // frame.setPayload(QByteArray::fromHex("0")); //send 0 to de-activate
                localGPIODataSentAlreadyfor0 = true;
                localGPIODataSentAlreadyfor1 = false;
                SendGPIOSignal = false;
                // frame.setFrameType(QCanBusFrame::DataFrame);
                // canBusDevice->writeFrame(frame);
                std::thread t([]() {
                    delayedSetGpio(17, false);
                });

                // Detach the thread so it runs independently
                t.detach();
            }
            else if(!localGPIODataSentAlreadyfor1 && SendGPIOSignal)
            {
                // QCanBusFrame frame;
                // frame.setFrameId(0x1999);
                // frame.setPayload(QByteArray::fromHex("1")); //send 1 to activate
                localGPIODataSentAlreadyfor1 = true;
                localGPIODataSentAlreadyfor0 = false;
                SendGPIOSignal = false;
                // frame.setFrameType(QCanBusFrame::DataFrame);
                // canBusDevice->writeFrame(frame);
                // QThread::msleep(static_cast<unsigned long>(3000));
                // Start a new thread and pass arguments using lambda
                std::thread t([]() {
                    delayedSetGpio(17, true);
                });

                // Detach the thread so it runs independently
                t.detach();
            }
        }
    }
};

class CanMessageSenderGroup2 : public QThread {
    Q_OBJECT
public:
    CanMessageSenderGroup2(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages)
        : fileParser(parser), messages(messages) {}

protected:
    void run() override {
        sendShmSDMMessage(fileParser, messages);
    }

private:
    QCanDbcFileParser& fileParser;
    std::unordered_map<std::string, Message>& messages;

    void sendShmSDMMessage(QCanDbcFileParser& fileParser, std::unordered_map<std::string, Message>& messages) {

        std::unordered_map<std::string, std::chrono::steady_clock::time_point> nextSendTime;
        for (auto& pair : messages) {
            nextSendTime[pair.first] = std::chrono::steady_clock::now();
        }

        QCanBusDevice* canBusDevice = QCanBus::instance()->createDevice("socketcan", "can0");
        canBusDevice->connectDevice();
        while (true) {

            if(!IsDataTransmissionStarted)
            {
                //reset timing
                for (auto& pair : messages) {
                    nextSendTime[pair.first] = std::chrono::steady_clock::now();
                }
                continue;
            }

            // Prepare SHM and SDM data and send them via CAN bus
            for (auto& pair : messages) {

                auto& message = pair.second;
                //if not connected for driving dont send this message


                std::string messageId = pair.first;
                // if (message.sheetName != "Main")
                //     continue;

                auto now = std::chrono::steady_clock::now();
                if (now >= nextSendTime[messageId]) {
                    auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(now - nextSendTime[messageId]);
                    // Add the cycleTime to the timeDifference
                    auto totalTime = timeDifference + std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));

                    // stream << totalTime.count() << ","
                    //        <<  QString::number(message.messageId29Bit, 16).toUpper() << "\n";
                    CreateSDMData(fileParser, messages);

                    if(message.sheetName == "Main")
                        CreateSHMData(pair, message.shmCounter);

                    if(message.sheetName == "Main")
                    {
                        std::string target_node = "Vehicle_Control_Unit";
                        for (const auto& msg : fileParser.messageDescriptions()) {
                            if (msg.transmitter() == target_node && message.SHM[0].message == msg.name()) {

                                quint32 shmId = static_cast<quint32>(msg.uniqueId());
                                QByteArray shmDataArray(reinterpret_cast<const char*>(message.shmSignedData.data()), msg.size());
                                QCanBusFrame shmFrame(shmId, shmDataArray);
                                shmFrame.setFrameType(QCanBusFrame::DataFrame);
                                // if(msg.name()!="S1_ES_PGN61427_SHM_Rx")
                                canBusDevice->writeFrame(shmFrame);


                                nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SHM[0].cycleTime));

                                QThread::msleep(static_cast<unsigned long>(message.SDM[0].deltaTime));

                                QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), msg.size()));
                                sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                                canBusDevice->writeFrame(sdmFrame);
                                message.incrementCounter();
                                break;
                            }
                        }
                    }
                    else{
                        // version 2 changes using dlc from the dbc to send only data in the payload
                        std::string target_node = "Vehicle_Control_Unit";
                        for (const auto& msg : fileParser.messageDescriptions()) {
                            if (msg.transmitter() == target_node && message.SDM[0].message == msg.name()) {
                                nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));
                                QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), msg.size()));
                                sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                                canBusDevice->writeFrame(sdmFrame);
                                break;
                            }
                        }

                    }
                }
            }
        }
    }
};


class MainController : public QObject
{
    Q_OBJECT

public:
    explicit MainController(SignalValueMap& signalValueMapInstance, QObject *parent = nullptr)
        : QObject(parent), m_signalValueMapInstance(signalValueMapInstance) {}
    // Make the function invokable using Q_INVOKABLE
    Q_INVOKABLE void toggleStartStop(bool checked) {
        if (checked) {
            IsDataTransmissionStarted = true;
            //logs
            QDateTime cstTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("America/Chicago"));
            logManager.addLog("Vehicle CAN Transmission Started for Battery initilisation at: "+ cstTime.toString());
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 3;

            // Write CSV header
            // stream << "Timestamp,CAN_ID,Data\n";


        } else {
            IsDataTransmissionStarted = false;
            HVReq=false;

            //this is critical as we need to make all the dictionary values invalid due to no CAN data being recieved otherwise it will take the old value and do state transition which is undesirable
            for (auto it = m_signalValueMapInstance.m_signalValueMap.begin(); it != m_signalValueMapInstance.m_signalValueMap.end(); ++it) {
                it->IsValid = false;
            }
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 0;

            // file.close();
            // qDebug() << "CAN data logged to can_log.csv";
        }
        SendGPIOSignal=true;
    }

    Q_INVOKABLE void toggleConnectDriving(bool checked) {
        if (checked) {
            IsConnectForDriving = true;
            dataVec["Vehicle_Control_Unit.PDU_Rx_1.Close_GroupA_Contactors"] = 1;
            dataVec["Vehicle_Control_Unit.PDU_Rx_1.Close_GroupB_Contactors"] = 1;
            // stream << "Timestamp,CAN_ID\n";

        } else {
            IsConnectForDriving = false;
            //sevcon inverter control word set for all the4 inverter
            dataVec["Vehicle_Control_Unit.PDU_Rx_1.Close_GroupA_Contactors"] = 0;
            dataVec["Vehicle_Control_Unit.PDU_Rx_1.Close_GroupB_Contactors"] = 0;
            //sevcon inverter control word set for all the 4 inverter
            dataVec["Vehicle_Control_Unit.HC1_Demands.ControlWord"] = 6;
            dataVec["Vehicle_Control_Unit.HC1_Demands_2.ControlWord"] = 6;
            dataVec["Vehicle_Control_Unit.HC1_Demands_3.ControlWord"] = 6;
            dataVec["Vehicle_Control_Unit.HC1_Demands_4.ControlWord"] = 6;

        }

    }

    Q_INVOKABLE void toggleACCharging(bool checked) {
        if (checked) {
            IsConnectForDriving = true;
            IsConnectedForACCharging=true;
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 2;


        } else {
            IsConnectForDriving = false;
            IsConnectedForACCharging=false;
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 0;

        }
    }

    Q_INVOKABLE void toggleDCCharging(bool checked) {
        if (checked) {
            IsConnectForDriving = true;
            IsConnectedForDCCharging=true;
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 5;

        } else {

            IsConnectForDriving = false;
            IsConnectedForDCCharging=false;
            dataVec["Vehicle_Control_Unit.S1_ES_PGN61427.Control_Es_Relay"] = 0;


        }
    }
    Q_INVOKABLE void toggleCrash(bool checked) {
        if (checked) {
            IsConnectForDriving = true;
            dataVec["Vehicle_Control_Unit.CN_PGN61483.Crash_Typ"] = 1;

        } else {
            IsConnectForDriving = false;
            dataVec["Vehicle_Control_Unit.CN_PGN61483.Crash_Typ"] = 0;

        }
    }
private:
    SignalValueMap& m_signalValueMapInstance;
};
void splitMessagesByNodeName(
    const std::unordered_map<std::string, Message>& inputMessages,
    const std::vector<std::string>& group1NodeNames,
    const std::vector<std::string>& group2NodeNames,
    std::unordered_map<std::string, Message>& group1Messages,
    std::unordered_map<std::string, Message>& group2Messages)
{
    for (const auto& [key, message] : inputMessages) {
        const std::string& node = message.nodeName;

        if (std::find(group1NodeNames.begin(), group1NodeNames.end(), node) != group1NodeNames.end()) {
            group1Messages[key] = message;
        } else if (std::find(group2NodeNames.begin(), group2NodeNames.end(), node) != group2NodeNames.end()) {
            group2Messages[key] = message;
        }
        else{
            qDebug()<<"Failed splitting message -- check here immediately";
        }
    }
}

int main(int argc, char *argv[]) {
    set_qt_environment();
    QGuiApplication app(argc, argv);

    // Enable Qt Virtual Keyboard
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("logManager", &logManager);

    WifiManager wifiManager;
    engine.rootContext()->setContextProperty("wifiManager", &wifiManager);
    // Register SystemManager for QML
    SystemManager systemManager;
    engine.rootContext()->setContextProperty("systemManager", &systemManager);
    logManager.addLog("Vehicle Initialised");

    TreeModel treemodel;
    engine.rootContext()->setContextProperty("treeModel", &treemodel);
    // Create and expose TimeManager to QML
    TimeManager timeManager;
    engine.rootContext()->setContextProperty("timeManager", &timeManager);
    SignalValueMap signalValueMapInstance;

    // Expose to QML
    engine.rootContext()->setContextProperty("signalValueMap", &signalValueMapInstance);
    qmlRegisterType<DatabaseManager>("com.example.db", 1, 0, "DatabaseManager");
    qmlRegisterType<MainController>("MainController", 1, 0, "MainController");
    MainController mainController(signalValueMapInstance);
    engine.rootContext()->setContextProperty("mainController", &mainController);


    const QUrl url(mainQmlFile);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);


    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    // if (QSqlDatabase::drivers().isEmpty()) {
    //     qDebug() << "No SQL drivers available!";
    //     return 1;
    // }
    // qDebug() << "Available SQL drivers:" << QSqlDatabase::drivers();



    // Create table
    DatabaseManager dbManager;
    //NodeName, SheetName, UniqueID, Message, Signal, DeltaTime, IsSHM,CycleTime,Value -- columns order


    // // Read data into model
    std::unordered_map<std::string, Message> messages;
    QSqlQueryModel *model = dbManager.readData();
    // Iterate through the rows in the model
    for (int row = 0; row < model->rowCount(); ++row) {
        QString sheetName = model->index(row, model->record().indexOf("SheetName")).data().toString();
        QString nodeName =  model->index(row, model->record().indexOf("NodeName")).data().toString();
        if (sheetName != "Main" && sheetName != "nonshm") {
            continue; // Skip rows not belonging to the specified sheets
        }

        std::string pgn;
        long double value = 0.0;
        std::string signal;
        long double cycleTime = 0.0;
        long double deltaTime = 0.0;
        bool isSHM = false;
        std::string messageName;

        // Access columns based on their names or indexes
        QVariant pgnVar = model->index(row, model->record().indexOf("UniqueID")).data();
        QVariant valueVar = model->index(row, model->record().indexOf("Value")).data();
        QVariant signalVar = model->index(row, model->record().indexOf("Signal")).data();
        QVariant cycleTimeVar = model->index(row, model->record().indexOf("CycleTime")).data();
        QVariant deltaTimeVar = model->index(row, model->record().indexOf("DeltaTime")).data();
        QVariant isSHMVar = model->index(row, model->record().indexOf("IsSHM")).data();
        QVariant messageNameVar = model->index(row, model->record().indexOf("Message")).data();

        // Convert QVariant values into the appropriate types
        pgn = pgnVar.toString().toStdString();
        value = valueVar.toDouble(); // Convert QVariant to long double
        signal = signalVar.toString().toStdString();
        cycleTime = cycleTimeVar.toDouble();
        deltaTime = deltaTimeVar.toDouble();

        // Check for NaN in deltaTime and replace with 0.0 if necessary
        if (std::isnan(deltaTime)) {
            deltaTime = 0.0;
        }

        isSHM = isSHMVar.toBool(); // Convert QVariant to bool
        messageName = messageNameVar.toString().toStdString();

        // Store the data in the map
        if (isSHM) {
            messages[pgn].SHM.emplace_back(value, cycleTime, deltaTime, signal, messageName);
        } else {
            messages[pgn].SDM.emplace_back(value, cycleTime, deltaTime, signal, messageName);
        }
        messages[pgn].sheetName = sheetName.toStdString();
        messages[pgn].nodeName = nodeName.toStdString();

    }
    // printModelData(model);
    // Output results for debugging
    for (const auto& pair : messages) {
        const auto& pgn = pair.first;
        const auto& message = pair.second;

        qDebug() << "----------------------------------------";
        qDebug() << "PGN:" << pgn;
        qDebug() << "Sheet Name:" << QString::fromStdString(message.sheetName);
        qDebug() << "Node Name:" << QString::fromStdString(message.nodeName);

        qDebug() << "\nSHM Entries:";
        if (message.SHM.empty()) {
            qDebug() << "  No SHM entries.";
        } else {
            for (const auto& shmEntry : message.SHM) {
                qDebug().nospace()
                << "  Value: " << QString::number(shmEntry.value, 'f', 5)
                << ", Cycle Time: " << QString::number(shmEntry.cycleTime, 'f', 5)
                << ", Delta Time: " << QString::number(shmEntry.deltaTime, 'f', 5)
                << ", Signal: " << QString::fromStdString(shmEntry.signal)
                << ", Message: " << QString::fromStdString(shmEntry.message);
            }
        }

        qDebug() << "\nSDM Entries:";
        if (message.SDM.empty()) {
            qDebug() << "  No SDM entries.";
        } else {
            for (const auto& sdmEntry : message.SDM) {
                qDebug().nospace()
                << "  Value: " << QString::number(sdmEntry.value, 'f', 5)
                << ", Cycle Time: " << QString::number(sdmEntry.cycleTime, 'f', 5)
                << ", Delta Time: " << QString::number(sdmEntry.deltaTime, 'f', 5)
                << ", Signal: " << QString::fromStdString(sdmEntry.signal)
                << ", Message: " << QString::fromStdString(sdmEntry.message);
            }
        }
    }

    qDebug() << "----------------------------------------";

    // if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //     qCritical() << "Error opening file at:" << file.fileName();
    // }

    // Parse DBC file
    QCanDbcFileParser fileParser;
    QString path = "/root/vehicle.dbc";
    QString str = QDir::toNativeSeparators(path); // Converts to correct format

    const bool result = fileParser.parse(str);
    if (result) {
        logManager.addLog("DBC File Parsed Successfully:"+ path);
    } else {
        logManager.addLog("Failed Parsing. Error:"+ fileParser.errorString());

    }
    std::unordered_map<std::string, Message> nonMainMessages;

    for (auto& pair : messages) {
        auto& message = pair.second;
        if (message.sheetName != "Main") {
            nonMainMessages[pair.first] = message;// Add the message to the vector
        }
    }
    QJsonObject fsm = loadFSMFromFile("/root/fsm.json");

    TimeDateResponder responder(signalValueMapInstance.m_signalValueMap,fsm);
    responder.initialize();




    //fix to split the messages based on nodename column and spawn the new thread after splitting
    // group 1:  vig, pdu, obc -- power data together as they are time specific
    // group 2:  hvlp1, hvlp2, hvlp3, hvlp4, editron -- basically all inverter data together
    std::vector<std::string> group1 = {"vig"};
    std::vector<std::string> group2 = {"hvlp1", "hvlp2", "hvlp3", "hvlp4", "editron", "pdu", "obc"};

    std::unordered_map<std::string, Message> group1Messages;
    std::unordered_map<std::string, Message> group2Messages;

    splitMessagesByNodeName(messages, group1, group2, group1Messages, group2Messages);

    // Initialize the CanMessageSender thread
    CanMessageSenderGroup1 senderGroup1(fileParser, group1Messages); // main thread handling logic flow
    senderGroup1.start();  // This starts the thread

    //below thread sends group2 message data -- if you change logic in the above thread make sure to revisit here
    CanMessageSenderGroup2 senderGroup2(fileParser, group2Messages); // main thread handling logic flow
    senderGroup2.start();  // This starts the thread



    //todo handle current discharge and charge basically forward the battery limit to the motors by dividing them properly
    //handle regen if required declare the variable here and handle the functionality
    //dynamic motors are available in the vehicle controls tab

    //how to handle sequence of value change like a state machine transition: for each event like when vehicle start, vehicle stop to charging
    //connect for driving handles both charging, driving or regen


    // CanNonSHMMessageSender nonShmSender(fileParser,messages);
    // nonShmSender.initialize();
    // Simulate log addition every second


    return app.exec();
}

#include "main.moc" // Add this if Q_OBJECT is used
