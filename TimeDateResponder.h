#ifndef TIMEDATERESPONDER_H
#define TIMEDATERESPONDER_H

#include <QObject>
#include <QCanBusDevice>
#include <QCanDbcFileParser>
#include <QCanBusFrame>
#include <QDateTime>
#include <QVector>
#include "LogManager.h"  // Assuming you have a LogManager class for logging
#include "fa.h"

class TimeDateResponder : public QObject {
    Q_OBJECT

public:
    explicit TimeDateResponder(QCanDbcFileParser& parser, std::unordered_map<std::string, Message>& messages, QObject* parent = nullptr);

    void initialize();

private:
    QCanBusDevice *canDevice = nullptr;
    QCanBusDevice *canDevice1 = nullptr;

    QCanDbcFileParser& fileParser;
    std::unordered_map<std::string, Message>& messages;

    LogManager logManager; // Assuming LogManager is your logging handler

    void processFrames();
    bool isTimeDateRequest(const QCanBusFrame& frame);
    QCanBusFrame createTimeDateResponse();
};

#endif // TIMEDATERESPONDER_H
