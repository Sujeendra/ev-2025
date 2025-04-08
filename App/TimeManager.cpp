#include "TimeManager.h"

TimeManager::TimeManager(QObject *parent) : QObject(parent) {
    // Timer to update the time every second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TimeManager::updateTime);
    timer->start(1000);
    updateTime(); // Set initial time
}

QString TimeManager::currentTime() const {
    return m_currentTime;
}

void TimeManager::updateTime() {
    QDateTime cstTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("America/Chicago"));
    m_currentTime = cstTime.toString("MM-dd-yyyy hh:mm:ss AP");
    emit timeChanged();
}
