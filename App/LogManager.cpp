#include "LogManager.h"

LogManager::LogManager(QObject* parent) : QObject(parent) {}

void LogManager::addLog(const QString& log) {
    emit newLogAdded(log); // Notify QML to append the latest log
}
