#include "CanWorker.h"
#include <QCanBus>
#include <QDebug>

CanWorker::CanWorker()
{
    canDevice = QCanBus::instance()->createDevice("socketcan", "can0");
    connect(canDevice, &QCanBusDevice::framesReceived, this, &CanWorker::processFrames);
}

void CanWorker::connectToDevice()
{
    if (canDevice->connectDevice()) {
        qDebug() << "CAN device connected.";
    }
}

void CanWorker::processFrames()
{
    while (canDevice->framesAvailable()) {
        QCanBusFrame frame = canDevice->readFrame();
        emit frameProcessed(frame);  // Emit the frame to the main thread
    }
}
