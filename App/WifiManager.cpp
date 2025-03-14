#include "WifiManager.h"

WifiManager::WifiManager(QObject *parent) : QObject(parent) {}

void WifiManager::connectToWifi(const QString &ssid, const QString &password) {
    QProcess process;
    process.start("sh", QStringList() << "-c" << QString("wpa_passphrase \"%1\" \"%2\" > /etc/wpa_supplicant.conf").arg(ssid, password));
    process.waitForFinished();

    process.start("wpa_supplicant", QStringList() << "-B" << "-i" << "wlan0" << "-c" << "/etc/wpa_supplicant.conf");
    process.waitForFinished();
    if (process.exitCode() == 0) {
        QString ip = getIPAddress();
        emit wifiConnected(ssid, ip);
    } else {
        emit wifiFailed();
    }
}

QString WifiManager::getIPAddress() {
    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol &&
                entry.ip() != QHostAddress::LocalHost) {
                return entry.ip().toString();
            }
        }
    }
    return "Not Assigned";
}
