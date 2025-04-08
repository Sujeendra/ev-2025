#include "SystemManager.h"

SystemManager::SystemManager(QObject *parent) : QObject(parent) {}

void SystemManager::restartDevice() {
    QProcess::execute("reboot");  // For Linux-based systems
}

void SystemManager::shutdownDevice() {
    QProcess::execute("shutdown");
}
