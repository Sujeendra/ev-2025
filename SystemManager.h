#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <QObject>
#include <QProcess>

class SystemManager : public QObject {
    Q_OBJECT
public:
    explicit SystemManager(QObject *parent = nullptr);

    Q_INVOKABLE void restartDevice();
    Q_INVOKABLE void shutdownDevice();
};

#endif // SYSTEMMANAGER_H
