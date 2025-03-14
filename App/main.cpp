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
struct VehicleTabData {
    int32_t pgn;        // PGN as an integer
    double value;       // Value field
    char signal[50];    // Array for signal as string
    bool IsSent = false; //used for custom logic --> for now used to send different values at different instances
};
std::vector<VehicleTabData> dataVec(10);
bool IsConnectForDriving = false;
bool IsDataTransmissionStarted = false;
bool IsConnectedForDCCharging = false;
bool IsConnectedForACCharging=false;
LogManager logManager;
bool SendGPIOSignal = false;
bool HVReq=false;
bool IsBatteryContactorClosed = false;
std::vector<uint32_t> frameIdsToForward = {0x1918FF71, 0x1919FF71, 0x191AFF71, 0xCFF91FD, 0xCFF92FD}; // both  sevcon hvlp and editron inverter data is forwarded but hvlp one is assumed we have to configure it for three  todo



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
                // Set message ID
                quint32 uniqueId = static_cast<quint32>(description.uniqueId());
                (*result).messageId29Bit =static_cast<int32_t>(uniqueId & 0x7FFFFFFF); // 29-bit message ID

                // Loop over all signal descriptions in the message
                for (const auto& signal : description.signalDescriptions()) {
                    auto value_it = std::find_if((*result).SDM.begin(), (*result).SDM.end(),
                                                 [&signal](const DataEntry& entry) {
                                                     return entry.signal == signal.name().toStdString();
                                                 });

                    // Check if the element was found in SDM
                    if (value_it != (*result).SDM.end()) {
                        // Add check to read value from shared data if it has changed there
                        for (int i = 0; i < 6; ++i) {
                            if ((*result).messageId29Bit == dataVec[i].pgn && signal.name().toStdString() == dataVec[i].signal) {

                                //here check for the IsSent variable to change value dynamically -- todo: check the logic again
                                // if(dataVec[i].pgn == 0xCEFF321){
                                //     //basically sending disconnect once before sending connect for driving or charging as per the manual
                                //     if(IsConnectForDriving || IsConnectedForACCharging || IsConnectedForDCCharging){
                                //         if(!dataVec[i].IsSent){
                                //             dataVec[i].value = 0;
                                //             dataVec[i].IsSent = true;
                                //         }
                                //         else{
                                //             if(IsConnectForDriving)
                                //                 dataVec[i].value = 3;
                                //             else if(IsConnectedForACCharging)
                                //                 dataVec[i].value = 2;
                                //             else if(IsConnectedForDCCharging)
                                //                 dataVec[i].value = 5;
                                //         }
                                //     }
                                // }
                                value_it->value = dataVec[i].value;
                            }
                        }

                        // If the value is found, process the signal encoding
                        if (value_it != (*result).SDM.end()) {
                            double value = value_it->value;
                            // Encode the signal value using the QtCanDbc signal encoding method
                            encodeSignal(value, signal.offset(), signal.factor(), signal.startBit(), signal.bitLength(), (*result).sdmSignedData, signal.dataEndian(),signal.dataFormat());
                        }
                    }
                }
            }
        }
    }
}
class TimeDateResponder : public QObject {
    Q_OBJECT

public:
    explicit TimeDateResponder(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages)
        : fileParser(parser), messages(messages) {}

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
        // Connect to frame reception
        connect(canDevice, &QCanBusDevice::framesReceived, this, &TimeDateResponder::processFrames);
    }

private:
    QCanBusDevice *canDevice = nullptr;
    QCanBusDevice *canDevice1 = nullptr;

    QCanDbcFileParser& fileParser;
    std::unordered_map<std::string, Message>& messages;

    void processFrames() {

        std::unordered_map<std::string, std::chrono::steady_clock::time_point> nextSendTime;
        std::vector<Message> nonMainMessages; // Container to store messages from non-"Main" sheets


        for (auto& pair : messages) {
            if (!pair.second.SHM.empty()) {
                nextSendTime[pair.first] = std::chrono::steady_clock::now();
            }
        }


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

            if (frame.frameId() == 0x18EF21F3 && dataVec[5].value == 0){
                std::string target_node = "VIC";
                for (const auto& message : fileParser.messageDescriptions()) {
                    if (message.transmitter() == target_node && message.name() == "PropA_C3_PGN61184") {
                        for (const auto& signal : message.signalDescriptions()) {
                            if(signal.name() == "IChLimLong" ||  signal.name() == "UChLimLong")
                            {
                                double value=decodeSignal(reinterpret_cast<const uint8_t*>(frame.payload().constData()),signal.offset(), signal.factor(), signal.startBit(), signal.bitLength(),signal.dataEndian(),signal.dataFormat());
                                if(signal.name() == "IChLimLong"){
                                    dataVec[4].value = value;
                                }
                                else{
                                    dataVec[3].value = value;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            else if (frame.frameId() == 0x14F096F3){
                std::string target_node = "VIC";
                for (const auto& message : fileParser.messageDescriptions()) {
                    if (message.transmitter() == target_node && message.name() == "HVESSS1_PGN61590") {
                        int count=0;
                        for (const auto& signal : message.signalDescriptions()) {

                            if(signal.name() == "HS_IntChrgrSts")
                            {
                                double value=decodeSignal(reinterpret_cast<const uint8_t*>(frame.payload().constData()),signal.offset(), signal.factor(), signal.startBit(), signal.bitLength(),signal.dataEndian(),signal.dataFormat());
                                //enable or disable charging
                                if(value == 1 && IsConnectedForACCharging)
                                    dataVec[5].value = 0;
                                else
                                    dataVec[5].value = 1;

                                if(IsConnectedForDCCharging && value==1)
                                    dataVec[2].value = 1;
                                else
                                    dataVec[2].value = 0;

                                count++;
                            }
                            if(signal.name() == "HS_HiUBusCnctnSts")
                            {
                                double value=decodeSignal(reinterpret_cast<const uint8_t*>(frame.payload().constData()),signal.offset(), signal.factor(), signal.startBit(), signal.bitLength(),signal.dataEndian(),signal.dataFormat());
                                //this is for taking action of sending connect for driving or charging can is initialised
                                if(value==1)
                                {
                                    IsBatteryContactorClosed = true; // use this variable for additional logic build as shown below
                                    //sequence transition of sevcon inverter from shutdown to energise to run only when battery contactors are closed
                                    if(dataVec[9].value == 6)
                                    {
                                        dataVec[9].value=3;
                                        dataVec[8].value=3;
                                        dataVec[7].value=3;
                                        dataVec[6].value=3;
                                    }
                                    //high chance this might not sink properly with the sending or receving node in that case read or decode value from sevcon signal
                                    else if(dataVec[9].value == 3)
                                    {
                                        dataVec[9].value=5;
                                        dataVec[8].value=5;
                                        dataVec[7].value=5;
                                        dataVec[6].value=5;
                                    }

                                }
                                else if(value == 0)
                                {
                                    HVReq = true;
                                    IsBatteryContactorClosed = false;
                                }
                                else
                                    IsBatteryContactorClosed = false;

                                count++;
                            }
                            if(count==2)
                                break;
                        }
                        break;
                    }
                }
            }


            // frame forward
            if (std::find(frameIdsToForward.begin(), frameIdsToForward.end(), frame.frameId()) != frameIdsToForward.end()) {
                // Forward frame to can1
                canDevice1->writeFrame(frame);
            }
            // Prepare SDM data and send them via CAN bus
            // for (auto& pair : messages) {

                //     auto& message = pair.second;
                //     std::string messageId = pair.first;

            //     auto now = std::chrono::steady_clock::now();
            //     if (now >= nextSendTime[messageId]) {
            //         CreateSDMData(fileParser, messages);
            //         QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), message.sdmSignedData.size()));
            //         sdmFrame.setFrameType(QCanBusFrame::DataFrame);
            //         canDevice->writeFrame(sdmFrame);
            //         nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));
            //     }
            // }
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
class CanMessageSender : public QThread {
    Q_OBJECT
public:
    CanMessageSender(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages)
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
                if(message.SDM[0].message == "S1_ES_PGN61427" && !IsConnectForDriving && !HVReq)
                {
                    nextSendTime[pair.first] = std::chrono::steady_clock::now();
                    continue;
                }
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
                                QByteArray shmDataArray(reinterpret_cast<const char*>(message.shmSignedData.data()), message.shmSignedData.size());
                                QCanBusFrame shmFrame(shmId, shmDataArray);
                                shmFrame.setFrameType(QCanBusFrame::DataFrame);
                                // if(msg.name()!="S1_ES_PGN61427_SHM_Rx")
                                    canBusDevice->writeFrame(shmFrame);


                                nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SHM[0].cycleTime));

                                QThread::msleep(static_cast<unsigned long>(message.SDM[0].deltaTime));

                                QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), message.sdmSignedData.size()));
                                sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                                canBusDevice->writeFrame(sdmFrame);
                                message.incrementCounter();
                                break;
                            }
                        }
                    }
                    else{
                        nextSendTime[messageId] += std::chrono::milliseconds(static_cast<int>(message.SDM[0].cycleTime));
                        QCanBusFrame sdmFrame(message.messageId29Bit, QByteArray(reinterpret_cast<const char*>(message.sdmSignedData.data()), message.sdmSignedData.size()));
                        sdmFrame.setFrameType(QCanBusFrame::DataFrame);
                        canBusDevice->writeFrame(sdmFrame);

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

// class CanNonSHMMessageSender : public QObject {
//     Q_OBJECT

// public:
//     CanNonSHMMessageSender(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages)
//         : fileParser(parser), messages(messages) {}

//     void initialize() {
//         // Initialize CAN Bus Device
//         canDevice = QCanBus::instance()->createDevice("socketcan", "can0");
//         if (!canDevice) {
//             qWarning() << "Error: Could not create CAN device.";
//             return;
//         }

//         if (!canDevice->connectDevice()) {
//             qWarning() << "Error: Could not connect to CAN device.";
//             return;
//         }

//         qDebug() << "CAN device connected successfully.";

//         // Connect to frame reception
//         connect(canDevice, &QCanBusDevice::framesReceived, this, &CanNonSHMMessageSender::processFrames);
//     }

// private:
//     QCanBusDevice *canDevice = nullptr;
//     QCanDbcFileParser& fileParser;
//     std::unordered_map<std::string, Message>& messages;

//     void processFrames() {

//     }



// };

class MainController : public QObject
{
    Q_OBJECT

public:
    explicit MainController(QObject *parent = nullptr) : QObject(parent) {}

    // Make the function invokable using Q_INVOKABLE
    Q_INVOKABLE void toggleStartStop(bool checked) {
        if (checked) {
            IsDataTransmissionStarted = true;
            //logs
            QDateTime cstTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("America/Chicago"));
            logManager.addLog("Vehicle CAN Transmission Started for Battery initilisation at: "+ cstTime.toString());

            // Write CSV header
            // stream << "Timestamp,CAN_ID,Data\n";


        } else {
            IsDataTransmissionStarted = false;
            HVReq=false;
            // file.close();
            // qDebug() << "CAN data logged to can_log.csv";
            // dataVec[1].IsSent = false;
        }
        SendGPIOSignal=true;
    }

    Q_INVOKABLE void toggleConnectDriving(bool checked) {
        if (checked) {
            dataVec[1].value = 3;
            IsConnectForDriving = true;
            // stream << "Timestamp,CAN_ID\n";

        } else {
            IsConnectForDriving = false;
            dataVec[1].value = 0;
            // dataVec[1].IsSent = false;
            //sevcon inverter control word set for all the4 inverter
            dataVec[9].value = 6;
            dataVec[8].value = 6;
            dataVec[7].value = 6;
            dataVec[6].value = 6;
        }

    }

    Q_INVOKABLE void toggleACCharging(bool checked) {
        if (checked) {
            dataVec[1].value = 2;
            IsConnectForDriving = true;
            IsConnectedForACCharging=true;

        } else {
            dataVec[1].value = 0;
            // dataVec[1].IsSent = false;
            IsConnectForDriving = false;
            IsConnectedForACCharging=false;
        }
    }

    Q_INVOKABLE void toggleDCCharging(bool checked) {
        if (checked) {
           dataVec[1].value = 5;
           // dataVec[2].value = 1;
           IsConnectForDriving = true;
           IsConnectedForDCCharging=true;
        } else {
            dataVec[1].value = 0;
            // dataVec[1].IsSent = false;
            // dataVec[2].value = 0;
            IsConnectForDriving = false;
            IsConnectedForDCCharging=false;

        }
    }
    Q_INVOKABLE void toggleCrash(bool checked) {
        if (checked) {
            dataVec[0].value = 1;
            IsConnectForDriving = true;
        } else {
            dataVec[0].value = 0;
            IsConnectForDriving = false;
        }
    }
};


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
    MainController mainController;
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

    // dbManager.createTable();
    // // Insert records
    // // dbManager.removeAllRecords();
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Control_Es_Relay", "0.292", "False", "20", "3");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Reserved_1_PGN61427", "0.292", "False", "20", "1");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Service_Mode_Request", "0.292", "False", "20", "3");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Reserved_2_PGN61427", "0.292", "False", "20", "429496729"); //keep max value from dbc
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Terminal_Voltage_Actual", "0.292", "False", "20", "2000");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "Counter_PGN61427", "0.292", "False", "20", "15");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "CRC_PGN61427", "0.292", "False", "20", "255");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427", "ShutDown_Clearance", "0.292", "False", "20", "3");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Inv_SDM_Data_Pg_Rx_61427", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Inv_SDM_E_Data_Pg_Rx_61427", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "SHM_Reserved_Rx_61427", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Sequence_Number_Rx_61427", "nan", "True", "20", "15");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Inv_SDM_SA_Rx_61427", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Inv_SDM_PS_Value_Rx_61427", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "Inv_SDM_PF_Value_Rx_61427", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61427", "S1_ES_PGN61427_SHM_Rx", "SDM_Data_CRC_Rx_61427", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_HiUBusCnctCmd_Rx1", "0.567", "False", "20", "1");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_PwrDwnCmd_Rx1", "0.567", "False", "20", "3");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_HiUBusAcvIslnTestCmd_Rx1", "0.567", "False", "20", "2");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_HiUBusPasIslnTestCmd_Rx1", "0.567", "False", "20", "2");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_CellBalnCmd_Rx1", "0.567", "False", "20", "3");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_EnaIntChrgrCmd_Rx1", "0.567", "False", "20", "3");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_OperConsent_Rx1", "0.567", "False", "20", "1");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_HiUBusHiSideRestrCnctReq_Rx1", "0.567", "False", "20", "3");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_HiUBusLoSideRestrCnctReq_Rx1", "0.567", "False", "20", "3");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_Ctl1Ctr_Rx1", "0.567", "False", "20", "15");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_Rx1", "HS_Ctl1CRC_Rx1", "0.567", "False", "20", "255");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Inv_SDM_Data_Pg_Rx1_6912", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Inv_SDM_E_Data_Pg_Rx1_6912", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "SHM_Reserved_Rx1_6912", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Sequence_Number_Rx1_6912", "nan", "True", "20", "23");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Inv_SDM_SA_Rx1_6912", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Inv_SDM_PS_Value_Rx1_6912", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "Inv_SDM_PF_Value_Rx1_6912", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "6912", "HVESSC1_PGN6912_SHM_Rx1", "SDM_Data_CRC_Rx1_6912", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483", "Crash_Typ", "2", "False", "20", "0");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483", "Crash_Ctr", "2", "False", "20", "11");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483", "Crash_Cks", "2", "False", "20", "4");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Inv_SDM_Data_Pg_Rx_61483", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Inv_SDM_E_Data_Pg_Rx_61483", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "SHM_Reserved_Rx_61483", "nan", "True", "20", "1");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Sequence_Number_Rx_61483", "nan", "True", "20", "27");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Inv_SDM_SA_Rx_61483", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Inv_SDM_PS_Value_Rx_61483", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "Inv_SDM_PF_Value_Rx_61483", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "61483", "CN_PGN61483_SHM_Rx", "SDM_Data_CRC_Rx_61483", "nan", "True", "20", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Two_Spd_Axle_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Parking_Brake_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Pause_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Park_Brake_Rel_Inhibit_Req", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Wheel_Based_Veh_Spd", "1.427", "False", "100", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Active", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Enable_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Brake_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Clutch_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_SetSwtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Coast_Decl_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Resume_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Accelerate_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_Set_Spd", "1.427", "False", "100", "255");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "PTO_Governor_State", "1.427", "False", "100", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Cruise_Control_States", "1.427", "False", "100", "7");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Engine_Idle_Increment_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Engine_Idle_Decrement_Switch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Engine_Diag_Test_Mode_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265", "Engine_Shutdwn_Ovride_Swtch", "1.427", "False", "100", "3");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Inv_SDM_Data_Pg_Rx_65265", "nan", "True", "100", "1");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Inv_SDM_E_Data_Pg_Rx_65265", "nan", "True", "100", "1");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "SHM_Reserved_Rx_65265", "nan", "True", "100", "1");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Sequence_Number_Rx_65265", "nan", "True", "100", "13");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Inv_SDM_SA_Rx_65265", "nan", "True", "100", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Inv_SDM_PS_Value_Rx_65265", "nan", "True", "100", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "Inv_SDM_PF_Value_Rx_65265", "nan", "True", "100", "0");
    // dbManager.insertRecord("Main", "65265", "CCVS1_PGN65265_SHM_Rx", "SDM_Data_CRC_Rx_65265", "nan", "True", "100", "0");
    // dbManager.insertRecord("nonshm", "600", "VCU", "VCU_DCDC_Command", "nan", "False", "200", "0");
    // dbManager.insertRecord("nonshm", "600", "VCU", "VCU_DCDC_SetV", "nan", "False", "200", "14");
    // dbManager.insertRecord("nonshm", "600", "VCU", "VCU_DCDC_MaxI", "nan", "False", "200", "10");
    // dbManager.insertRecord("nonshm", "0", "BMS", "BMS_Max_Voltage", "nan", "False", "1000", "300");
    // dbManager.insertRecord("nonshm", "0", "BMS", "BMS_Max_Current", "nan", "False", "1000", "60");
    // dbManager.insertRecord("nonshm", "0", "BMS", "BMS_Mode", "nan", "False", "1000", "0");
    // dbManager.insertRecord("nonshm", "0", "BMS", "BMS_Control", "nan", "False", "1000", "0");
    // editron and hvlp inverter database data below

    //SheetName, UniqueID, Message, Signal, DeltaTime, IsSHM,CycleTime,Value -- columns order
    //hvlp inverter migration --> set to torque mode
    // dbManager.insertRecord("nonshm", "1", "HC1_Demands", "Torque_Request", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "1", "HC1_Demands", "ControlWord", "nan", "False", "20", "5"); //transition from 6->3->5 when connected for driving only
    // dbManager.insertRecord("nonshm", "1", "HC1_Demands", "Torque_Traction_Limit", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "1", "HC1_Demands", "SEQ_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "1", "HC1_Demands", "CS_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "2", "HC2_Demands", "Torque_Regen_Limit", "nan", "False", "20", "-10");
    // dbManager.insertRecord("nonshm", "2", "HC2_Demands", "Speed_Limit_Forward", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "2", "HC2_Demands", "Speed_Limit_Reverse", "nan", "False", "20", "-100"); // we have to handle signed data properly for hvlp alone
    // dbManager.insertRecord("nonshm", "2", "HC2_Demands", "SEQ_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "2", "HC2_Demands", "CS_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "3", "HC3_Battery_Demands", "DC_Link_Imax_Discharge", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "3", "HC3_Battery_Demands", "DC_Link_Imax_Recharge", "nan", "False", "20", "-5");
    // dbManager.insertRecord("nonshm", "3", "HC3_Battery_Demands", "DC_Link_Voltage_Target", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "3", "HC3_Battery_Demands", "SEQ_CurrentLimits", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "3", "HC3_Battery_Demands", "CS_CurrentLimits", "nan", "False", "20", "0");

    // dbManager.insertRecord("nonshm", "4", "HC1_Demands_2", "Torque_Request", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "4", "HC1_Demands_2", "ControlWord", "nan", "False", "20", "5"); //may be we have to set 3 first and then 5 energise followed by enable
    // dbManager.insertRecord("nonshm", "4", "HC1_Demands_2", "Torque_Traction_Limit", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "4", "HC1_Demands_2", "SEQ_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "4", "HC1_Demands_2", "CS_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "5", "HC2_Demands_2", "Torque_Regen_Limit", "nan", "False", "20", "-10");
    // dbManager.insertRecord("nonshm", "5", "HC2_Demands_2", "Speed_Limit_Forward", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "5", "HC2_Demands_2", "Speed_Limit_Reverse", "nan", "False", "20", "-100"); // we have to handle signed data properly for hvlp alone
    // dbManager.insertRecord("nonshm", "5", "HC2_Demands_2", "SEQ_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "5", "HC2_Demands_2", "CS_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "6", "HC3_Battery_Demands_2", "DC_Link_Imax_Discharge", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "6", "HC3_Battery_Demands_2", "DC_Link_Imax_Recharge", "nan", "False", "20", "-5");
    // dbManager.insertRecord("nonshm", "6", "HC3_Battery_Demands_2", "DC_Link_Voltage_Target", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "6", "HC3_Battery_Demands_2", "SEQ_CurrentLimits", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "6", "HC3_Battery_Demands_2", "CS_CurrentLimits", "nan", "False", "20", "0");

    // dbManager.insertRecord("nonshm", "7", "HC1_Demands_3", "Torque_Request", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "7", "HC1_Demands_3", "ControlWord", "nan", "False", "20", "5"); //may be we have to set 3 first and then 5 energise followed by enable
    // dbManager.insertRecord("nonshm", "7", "HC1_Demands_3", "Torque_Traction_Limit", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "7", "HC1_Demands_3", "SEQ_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "7", "HC1_Demands_3", "CS_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "8", "HC2_Demands_3", "Torque_Regen_Limit", "nan", "False", "20", "-10");
    // dbManager.insertRecord("nonshm", "8", "HC2_Demands_3", "Speed_Limit_Forward", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "8", "HC2_Demands_3", "Speed_Limit_Reverse", "nan", "False", "20", "-100"); // we have to handle signed data properly for hvlp alone
    // dbManager.insertRecord("nonshm", "8", "HC2_Demands_3", "SEQ_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "8", "HC2_Demands_3", "CS_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "9", "HC3_Battery_Demands_3", "DC_Link_Imax_Discharge", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "9", "HC3_Battery_Demands_3", "DC_Link_Imax_Recharge", "nan", "False", "20", "-5");
    // dbManager.insertRecord("nonshm", "9", "HC3_Battery_Demands_3", "DC_Link_Voltage_Target", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "9", "HC3_Battery_Demands_3", "SEQ_CurrentLimits", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "9", "HC3_Battery_Demands_3", "CS_CurrentLimits", "nan", "False", "20", "0");


    // dbManager.insertRecord("nonshm", "10", "HC1_Demands_4", "Torque_Request", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "10", "HC1_Demands_4", "ControlWord", "nan", "False", "20", "5"); //may be we have to set 3 first and then 5 energise followed by enable
    // dbManager.insertRecord("nonshm", "10", "HC1_Demands_4", "Torque_Traction_Limit", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "10", "HC1_Demands_4", "SEQ_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "10", "HC1_Demands_4", "CS_Command1", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "11", "HC2_Demands_4", "Torque_Regen_Limit", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "11", "HC2_Demands_4", "Speed_Limit_Forward", "nan", "False", "20", "100");
    // dbManager.insertRecord("nonshm", "11", "HC2_Demands_4", "Speed_Limit_Reverse", "nan", "False", "20", "-100"); // we have to handle signed data properly for hvlp alone
    // dbManager.insertRecord("nonshm", "11", "HC2_Demands_4", "SEQ_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "11", "HC2_Demands_4", "CS_Command2", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "12", "HC3_Battery_Demands_4", "DC_Link_Imax_Discharge", "nan", "False", "20", "10");
    // dbManager.insertRecord("nonshm", "12", "HC3_Battery_Demands_4", "DC_Link_Imax_Recharge", "nan", "False", "20", "-5");
    // dbManager.insertRecord("nonshm", "12", "HC3_Battery_Demands_4", "DC_Link_Voltage_Target", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "12", "HC3_Battery_Demands_4", "SEQ_CurrentLimits", "nan", "False", "20", "0");
    // dbManager.insertRecord("nonshm", "12", "HC3_Battery_Demands_4", "CS_CurrentLimits", "nan", "False", "20", "0");

    // editron inverter migration -- migration done
    // dbManager.insertRecord("nonshm", "65312", "CMD_MOT_0", "cmd_mot_request", "nan", "False", "100", "1");
    // dbManager.insertRecord("nonshm", "65313", "CMD_MOT_1", "cmd_mot_ctrl_mode", "nan", "False", "10", "0"); //speed mode
    // dbManager.insertRecord("nonshm", "65313", "CMD_MOT_1", "cmd_mot_run", "nan", "False", "10", "1"); //set to run mode
    // dbManager.insertRecord("nonshm", "65314", "CMD_MOT_2", "cmd_mot_n_ref", "nan", "False", "100", "100"); // only this matters
    // dbManager.insertRecord("nonshm", "65314", "CMD_MOT_2", "cmd_mot_t_ref", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65314", "CMD_MOT_2", "cmd_mot_u_dc_ref", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65314", "CMD_MOT_2", "cmd_mot_p_ref", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65315", "CMD_MOT_3", "cmd_mot_f_ref", "nan", "False", "100", "10");  //not sure about the value
    // dbManager.insertRecord("nonshm", "65315", "CMD_MOT_3", "cmd_mot_pos_ref_frac", "nan", "False", "100", "10"); //not sure about the value
    // dbManager.insertRecord("nonshm", "65315", "CMD_MOT_3", "cmd_mot_pos_ref_full", "nan", "False", "100", "10"); //not sure about the value
    // dbManager.insertRecord("nonshm", "65316", "CMD_MOT_4", "cmd_mot_lim_n_max", "nan", "False", "100", "100"); // only this matters
    // dbManager.insertRecord("nonshm", "65316", "CMD_MOT_4", "cmd_mot_lim_n_min", "nan", "False", "100", "-100");
    // dbManager.insertRecord("nonshm", "65316", "CMD_MOT_4", "cmd_mot_lim_t_max", "nan", "False", "100", "100");
    // dbManager.insertRecord("nonshm", "65316", "CMD_MOT_4", "cmd_mot_lim_t_min", "nan", "False", "100", "-100");
    // dbManager.insertRecord("nonshm", "65317", "CMD_MOT_5", "cmd_mot_lim_u_dc_ovc_limit", "nan", "False", "100", "100"); //check the communication document for more info
    // dbManager.insertRecord("nonshm", "65317", "CMD_MOT_5", "cmd_mot_lim_u_dc_ovc_begin", "nan", "False", "100", "80");
    // dbManager.insertRecord("nonshm", "65317", "CMD_MOT_5", "cmd_mot_lim_u_dc_uvc_begin", "nan", "False", "100", "15");
    // dbManager.insertRecord("nonshm", "65317", "CMD_MOT_5", "cmd_mot_lim_u_dc_uvc_limit", "nan", "False", "100", "10");
    // dbManager.insertRecord("nonshm", "65318", "CMD_MOT_6", "cmd_mot_lim_p_max", "nan", "False", "100", "1000"); //check the communication document for more info
    // dbManager.insertRecord("nonshm", "65318", "CMD_MOT_6", "cmd_mot_lim_p_min", "nan", "False", "100", "800");
    // dbManager.insertRecord("nonshm", "65318", "CMD_MOT_6", "cmd_mot_lim_i_dc_max", "nan", "False", "100", "20");
    // dbManager.insertRecord("nonshm", "65318", "CMD_MOT_6", "cmd_mot_lim_i_dc_min", "nan", "False", "100", "10");
    // dbManager.insertRecord("nonshm", "65319", "CMD_MOT_7", "cmd_mot_speed_control_kp", "nan", "False", "100", "20"); // idk the gain value we have to check this -- kp control gain
    // dbManager.insertRecord("nonshm", "65319", "CMD_MOT_7", "cmd_mot_speed_control_ti", "nan", "False", "100", "20");
    // dbManager.insertRecord("nonshm", "65319", "CMD_MOT_7", "cmd_mot_speed_ref_ramp_time", "nan", "False", "100", "10");
    // dbManager.insertRecord("nonshm", "65320", "CMD_MOT_8", "cmd_mot_torque_rate_gen_side", "nan", "False", "100", "30");
    // dbManager.insertRecord("nonshm", "65320", "CMD_MOT_8", "cmd_mot_torque_rate_mot_side", "nan", "False", "100", "30");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_enable_resolver_commands", "nan", "False", "1000", "1");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_resolver_id_request", "nan", "False", "1000", "1");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_enable_resolver_feedback", "nan", "False", "1000", "1");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_save_resolver_parameters", "nan", "False", "1000", "0");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_resolver_id_method", "nan", "False", "1000", "0");
    // dbManager.insertRecord("nonshm", "65321", "CMD_MOT_9", "cmd_mot_resolver_offset", "nan", "False", "1000", "0");
    // dbManager.insertRecord("nonshm", "65296", "CMD_SYS_0", "cmd_sys_request", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65297", "CMD_SYS_1", "cmd_sys_fault_reset", "nan", "False", "10", "1");
    // dbManager.insertRecord("nonshm", "65297", "CMD_SYS_1", "cmd_sys_contactor", "nan", "False", "10", "0");
    // dbManager.insertRecord("nonshm", "65297", "CMD_SYS_1", "cmd_sys_control_switch", "nan", "False", "10", "0");
    // dbManager.insertRecord("nonshm", "65298", "CMD_SYS_2", "cmd_sys_digital_output_1", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65298", "CMD_SYS_2", "cmd_sys_digital_output_2", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65298", "CMD_SYS_2", "cmd_sys_digital_output_3", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65298", "CMD_SYS_2", "cmd_sys_digital_output_4", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65298", "CMD_SYS_2", "cmd_sys_digital_output_5", "nan", "False", "100", "0");

    //migration of epec pdu data
    // dbManager.insertRecord("nonshm", "65296", "PDU_Rx_1", "Close_GroupA_Contactors", "nan", "False", "100", "1");
    // dbManager.insertRecord("nonshm", "65296", "PDU_Rx_1", "Close_GroupB_Contactors", "nan", "False", "100", "1");
    // dbManager.insertRecord("nonshm", "65296", "PDU_Rx_1", "Disconnect_Rins_Meas", "nan", "False", "100", "0");
    // dbManager.insertRecord("nonshm", "65296", "PDU_Rx_1", "Reset_Faults", "nan", "False", "100", "1");


    // import ambient air temperature messages to be sent to the VIG
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269", "Amb_AirT", "1.427", "False", "1000", "25");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269", "Barometric_P", "1.427", "False", "1000", "30");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269", "Cab_InteriorT", "1.427", "False", "1000", "30");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269", "Eng_Intk1AirT", "1.427", "False", "1000", "30");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269", "Road_SurfT", "1.427", "False", "1000", "30");

    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Inv_SDM_Data_Pg_Rx_65269", "nan", "True", "1000", "1");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Inv_SDM_E_Data_Pg_Rx_65269", "nan", "True", "1000", "1");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "SHM_Reserved_Rx_65269", "nan", "True", "1000", "1");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Sequence_Number_Rx_65269", "nan", "True", "1000", "13");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Inv_SDM_SA_Rx_65269", "nan", "True", "1000", "0");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Inv_SDM_PS_Value_Rx_65269", "nan", "True", "1000", "0");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "Inv_SDM_PF_Value_Rx_65269", "nan", "True", "1000", "0");
    // dbManager.insertRecord("Main", "65269", "AMB_PGN65269_SHM_Rx", "SDM_Data_CRC_Rx_65269", "nan", "True", "1000", "0");


    //sending themral management system data to the ecu
    // dbManager.insertRecord("nonshm", "61184", "PropA_C4_PGN61184", "ITMSLimVeh", "nan", "False", "20", "4095");
    // dbManager.insertRecord("nonshm", "61184", "PropA_C4_PGN61184", "TMS_Active_Discharge", "nan", "False", "20", "3");
    //request pgn -- 64606 pgn state of charge
     // dbManager.insertRecord("nonshm", "59904", "RQST_ES_PGN59904_Rx1", "RQST_Rx1", "nan", "False", "5000", "64606");


    // Delete a record by condition
    // dbManager.deleteRecord("UniqueID = '1'");
    // dbManager.deleteRecord("UniqueID = '2'");
    // dbManager.deleteRecord("UniqueID = '3'");
    // dbManager.deleteRecord("UniqueID = '4'");
    // dbManager.deleteRecord("UniqueID = '5'");
    // dbManager.deleteRecord("UniqueID = '6'");
    // dbManager.deleteRecord("UniqueID = '7'");
    // dbManager.deleteRecord("UniqueID = '8'");
    // dbManager.deleteRecord("UniqueID = '9'");
    // dbManager.deleteRecord("UniqueID = '10'");
    // dbManager.deleteRecord("UniqueID = '11'");
    // dbManager.deleteRecord("UniqueID = '12'");

    //         dbManager.deleteRecord("UniqueID = '65312'");
    //         dbManager.deleteRecord("UniqueID = '65313'");
    //         dbManager.deleteRecord("UniqueID = '65314'");
    //         dbManager.deleteRecord("UniqueID = '65315'");
    //         dbManager.deleteRecord("UniqueID = '65316'");
    //         dbManager.deleteRecord("UniqueID = '65317'");
    //         dbManager.deleteRecord("UniqueID = '65318'");
    //         dbManager.deleteRecord("UniqueID = '65319'");
    //         dbManager.deleteRecord("UniqueID = '65320'");
    //         dbManager.deleteRecord("UniqueID = '65321'");
    //         dbManager.deleteRecord("UniqueID = '65296'");
    //         dbManager.deleteRecord("UniqueID = '65297'");
    //         dbManager.deleteRecord("UniqueID = '65298'");
    //         dbManager.deleteRecord("UniqueID = '0x01'");

    // // Remove all records
    // dbManager.removeAllRecords();
    // // Read data into model
    std::unordered_map<std::string, Message> messages;
    QSqlQueryModel *model = dbManager.readData();
    // Iterate through the rows in the model
    for (int row = 0; row < model->rowCount(); ++row) {
        QString sheetName = model->index(row, model->record().indexOf("SheetName")).data().toString();
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
    }
    // printModelData(model);
    // Output results for debugging
    for (const auto& pair : messages) {
        const auto& pgn = pair.first;
        const auto& message = pair.second;

        qDebug() << "----------------------------------------";
        qDebug() << "PGN:" << pgn;
        qDebug() << "Sheet Name:" << QString::fromStdString(message.sheetName);

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
    TimeDateResponder responder(fileParser,nonMainMessages);
    responder.initialize(); // Replace with your CAN interface name
    // Initialize the CanMessageSender thread
    CanMessageSender sender(fileParser, messages);
    sender.start();  // This starts the thread



    dataVec[0].pgn = 0x18F02B21;
    std::strcpy(dataVec[0].signal, "Crash_Typ");
    dataVec[0].value = 0;

    dataVec[1].pgn = 0xCEFF321;
    std::strcpy(dataVec[1].signal, "Control_Es_Relay");
    dataVec[1].value = 0;

    dataVec[2].pgn = 0x1800F5E5;
    std::strcpy(dataVec[2].signal, "VCU_DCDC_Command");
    dataVec[2].value = 0;

    dataVec[3].pgn = 0x1806E5F4;
    std::strcpy(dataVec[3].signal, "BMS_Max_Voltage");
    dataVec[3].value = 300;

    dataVec[4].pgn = 0x1806E5F4;
    std::strcpy(dataVec[4].signal, "BMS_Max_Current");
    dataVec[4].value = 60;

    dataVec[5].pgn = 0x1806E5F4;
    std::strcpy(dataVec[5].signal, "BMS_Control");
    dataVec[5].value = 1;

    //if battery contactors are closed then only send 6->3->5 sequence command to 4 sevcon inverter to turn on
    dataVec[6].pgn = 0x19107171;
    std::strcpy(dataVec[6].signal, "ControlWord");
    dataVec[6].value = 6;

    dataVec[7].pgn = 0x19107172;
    std::strcpy(dataVec[7].signal, "ControlWord");
    dataVec[7].value = 6;

    dataVec[8].pgn = 0x19107173;
    std::strcpy(dataVec[8].signal, "ControlWord");
    dataVec[8].value = 6;

    dataVec[9].pgn = 0x19107174;
    std::strcpy(dataVec[9].signal, "ControlWord");
    dataVec[9].value = 6;

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
