#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include <QTimer>

class TimeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)

public:
    explicit TimeManager(QObject *parent = nullptr);

    QString currentTime() const;

signals:
    void timeChanged();

private slots:
    void updateTime();

private:
    QString m_currentTime;
};

#endif // TIMEMANAGER_H
