#ifndef CANWORKER_H
#define CANWORKER_H

#include <QObject>
#include <QCanBusDevice>
#include <QCanBusFrame>

class CanWorker : public QObject
{
    Q_OBJECT

public:
    CanWorker();
    void connectToDevice();

public slots:
    void processFrames();

signals:
    void frameProcessed(const QCanBusFrame& frame);

private:
    QCanBusDevice* canDevice;
};

#endif // CANWORKER_H
