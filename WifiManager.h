#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <QObject>
#include <QProcess>
#include <QNetworkInterface>

class WifiManager : public QObject {
    Q_OBJECT
public:
    explicit WifiManager(QObject *parent = nullptr);

    Q_INVOKABLE void connectToWifi(const QString &ssid, const QString &password);
    Q_INVOKABLE QString getIPAddress();

signals:
    void wifiConnected(const QString &ssid, const QString &ip);
    void wifiFailed();
};

#endif // WIFIMANAGER_H
