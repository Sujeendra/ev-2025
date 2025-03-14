#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>

class LogManager : public QObject {
    Q_OBJECT

public:
    explicit LogManager(QObject* parent = nullptr);

    Q_INVOKABLE void addLog(const QString& log);

signals:
    void newLogAdded(const QString& log);
};

#endif // LOGMANAGER_H
