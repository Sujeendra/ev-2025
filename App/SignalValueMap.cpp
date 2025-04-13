#include "SignalValueMap.h"
#include <QDir>
#include <QDebug>
#include "utility.h"
#include <QElapsedTimer>

// // Start timers at initialization
// QElapsedTimer frameTimer;
// QElapsedTimer fpsTimer;
// int frameCount = 0;
// qint64 elapsedTime = 0;


SignalValueMap::SignalValueMap() {
    QString path = "/root/vehicle.dbc";  // Replace with actual path to your DBC file
    QString str = QDir::toNativeSeparators(path);

    const bool result = fileParser.parse(str);
    if (result) {
        qDebug() << "DBC File Parsed Successfully:" << path;
    } else {
        qDebug() << "Failed Parsing. Error:" << fileParser.errorString();
    }
    populateInitialDictionary();

    connect(&m_updateTimer, &QTimer::timeout, this, &SignalValueMap::updateDictionaryValues);
    m_updateTimer.start(2000);  // 2000ms interval // this is ok timing to avoid ui freezes as we will be using m_signalValueMap for finite state machine approach

    m_worker = new CanWorker();
    m_workerThread = new QThread();
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_worker, &CanWorker::connectToDevice);
    connect(m_worker, &CanWorker::frameProcessed, this, &SignalValueMap::processFrames);
    m_workerThread->start();
    // frameTimer.start();
    // fpsTimer.start();
}

QList<SignalValue*> SignalValueMap::signalValueList() const {
    return m_signalValueList;
}

void SignalValueMap::updateDictionaryValues() {



    // Simulate some values to verify the dashboard
    // m_signalValueMap["VIC.HVESSD1_PGN61584.HS_ULvl"].value = QRandomGenerator::global()->bounded(800);
    // m_signalValueMap["VIC.HVESSD1_PGN61584.HS_I"].value = QRandomGenerator::global()->bounded(200);
    // m_signalValueMap["VIC.HVESSD1_PGN61584.HS_AvlChPwr"].value = QRandomGenerator::global()->bounded(200);
    // m_signalValueMap["VIC.HVESSD1_PGN61584.HS_AvlDchaPwr"].value = QRandomGenerator::global()->bounded(200);
    // m_signalValueMap["OBC.OBC.OBC_Charger_Ready"].description = "Normal";
    // m_signalValueMap["OBC.OBC.OBC_Comm_Fault"].description = "Normal";
    // m_signalValueMap["VIC.HVESSD1_PGN61584.HS_ULvl"].description = "Normal";
    // m_signalValueMap["DCDC.DCDC.DC_Error_Communication"].description = "Fault";
    // m_signalValueMap["VIC.HVESSD2_PGN61585.HS_FastUpdSOC"].value = 85;
    // m_signalValueMap["VIC.HVESSHIST_PGN64606.HS_StofHealth"].value = 100;

    // Update the list for QML
    updateSignalValueList();

    emit signalValueMapChanged();

    // Measure frame time
    // qint64 frameTimeNs = frameTimer.nsecsElapsed();
    // frameTimer.restart();  // Restart for the next frame
    // double frameTimeMs = frameTimeNs / 1.0e6;  // Convert to milliseconds

    // // FPS calculation
    // frameCount++;
    // elapsedTime += fpsTimer.elapsed();
    // fpsTimer.restart();  // Restart FPS timer

    // if (elapsedTime > 900000) {  // Every 15 minutes (900 seconds)
    //     // Get current date and time
    //     QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    //     // Calculate FPS
    //     double fps = frameCount / (elapsedTime / 1000.0);

    //     // Print FPS with date and time
    //     qDebug() << "[" << currentTime << "] FPS:" << fps;

    //     // Reset counters
    //     frameCount = 0;
    //     elapsedTime = 0;
    // }


    // qDebug() << "Frame Time (ms):" << frameTimeMs;
}

void SignalValueMap::processFrames(const QCanBusFrame& frame) {
    for (const auto& message : fileParser.messageDescriptions()) {
        if (static_cast<quint32>(message.uniqueId()) == frame.frameId()) {
            QString transmitterName = message.transmitter();
            QString messageName = message.name();

            for (const auto& signal : message.signalDescriptions()) {
                QString signalName = signal.name();

                // Create the key: transmitterName.messageName.signalName
                QString key = QString("%1.%2.%3")
                                  .arg(transmitterName)
                                  .arg(messageName)
                                  .arg(signalName);

                qint32 value = decodeSignal(reinterpret_cast<const uint8_t*>(frame.payload().constData()),
                                            signal.offset(),
                                            signal.factor(),
                                            signal.startBit(),
                                            signal.bitLength(),
                                            signal.dataEndian(),
                                            signal.dataFormat());

                QString des = "";
                auto messageDescriptions = fileParser.messageValueDescriptions();
                if (messageDescriptions.contains(message.uniqueId())) {
                    const auto &signalDescriptions = messageDescriptions.value(message.uniqueId());

                    if (signalDescriptions.contains(signalName)) {
                        const auto &valueDescriptions = signalDescriptions.value(signalName);

                        if (valueDescriptions.contains(value)) {
                            des = valueDescriptions.value(value);
                        }
                    }
                }
                QString comment = signal.comment();
                m_signalValueMap[key] = SignalInfo(value, comment, des);
            }
            break;
        }
    }
}

void SignalValueMap::populateInitialDictionary() {
    for (const auto& message : fileParser.messageDescriptions()) {
        QString transmitterName = message.transmitter();
        QString messageName = message.name();

        for (const auto& signal : message.signalDescriptions()) {
            QString signalName = signal.name();

            // Create the key: transmitterName.messageName.signalName
            QString key = QString("%1.%2.%3")
                              .arg(transmitterName)
                              .arg(messageName)
                              .arg(signalName);

            // Initialize with a default value
            m_signalValueMap.insert(key, SignalInfo(0, "", ""));
        }
    }
    updateSignalValueList();
}

void SignalValueMap::updateSignalValueList() {
    for (auto it = m_signalValueMap.begin(); it != m_signalValueMap.end(); ++it) {
        bool found = false;

        for (SignalValue* signal : m_signalValueList) {
            if (signal->key() == it.key()) {
                signal->setValue(it.value().value);
                signal->setDescription(it.value().description);
                signal->setComment(it.value().comment);
                found = true;
                break;
            }
        }
        if (!found) {
            SignalValue* newSignal = new SignalValue();
            newSignal->setKey(it.key());
            newSignal->setValue(it.value().value);
            newSignal->setDescription(it.value().description);
            newSignal->setComment(it.value().comment);
            m_signalValueList.append(newSignal);
        }
    }
}
