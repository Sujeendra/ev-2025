#include "TimeDateResponder.h"
#include <QCanBus>
#include <QTimer>

TimeDateResponder::TimeDateResponder(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages, QObject* parent)
    : QObject(parent), fileParser(parser), messages(messages) {}

void TimeDateResponder::initialize() {
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

void TimeDateResponder::processFrames() {

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> nextSendTime;
    std::vector<Message> nonMainMessages; // Container to store messages from non-"Main" sheets


    for (auto& pair : messages) {
        if (!pair.second.SHM.empty()) {
            nextSendTime[pair.first] = std::chrono::steady_clock::now();
        }
    }
    while (canDevice->framesAvailable()) {
        QCanBusFrame frame = canDevice->readFrame();
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
                            //this is for taking action of sending connect for driving or chagring can is initialised
                            if(value == 0)
                                HVReq = true;
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
bool TimeDateResponder::isTimeDateRequest(const QCanBusFrame& frame) {
    return (frame.frameId() == 0xEA21F3 &&
            frame.payload()[0] == 0xE6 &&
            frame.payload()[1] == 0xFE &&
            frame.payload()[2] == 0x00);
}

QCanBusFrame TimeDateResponder::createTimeDateResponse() {
    QCanBusFrame frame;
    frame.setFrameId(0x18FEE621);
    frame.setFrameType(QCanBusFrame::DataFrame);

    QByteArray payload(8, 0);
    QDateTime utcTime = QDateTime::currentDateTimeUtc();

    // Populate UTC time in the payload
    payload[0] = utcTime.time().second() / 0.25;
    payload[1] = utcTime.time().minute();
    payload[2] = utcTime.time().hour();
    payload[3] = utcTime.date().month();
    payload[4] = utcTime.date().day() / 0.25;
    payload[5] = utcTime.date().year() - 1985;

    // Set offset values
    payload[6] = 125;
    payload[7] = 125;

    frame.setPayload(payload);
    return frame;
}
