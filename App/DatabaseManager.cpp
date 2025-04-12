#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");

    if (!db.open()) {
        qWarning() << "Database connection failed!";
    }

    // Set up the model
    tableModel = new QSqlTableModel(this, db);
    tableModel->setTable("records");
    tableModel->select();  // Load the data into the model
}

DatabaseManager::~DatabaseManager()
{
    db.close();
}

QSqlTableModel* DatabaseManager::getTableModel()
{
    return tableModel;
}

void DatabaseManager::updateRecord(int row, const QString &columnName, const QVariant &newValue)
{
    tableModel->setData(tableModel->index(row, tableModel->fieldIndex(columnName)), newValue);
    tableModel->submitAll();  // Commit the change to the database
}

bool DatabaseManager::createTable()
{
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS records ("
                              "NodeName TEXT, "
                              "SheetName TEXT, "
                              "UniqueID TEXT, "
                              "Message TEXT, "
                              "Signal TEXT, "
                              "DeltaTime TEXT, "
                              "IsSHM TEXT, "
                              "CycleTime TEXT, "
                              "Value TEXT)");


    if (!success) {
        qDebug() << "Error: Failed to create table" << query.lastError().text();
    }
    return success;
}

bool DatabaseManager::deleteTable()
{
    QSqlQuery query;
    bool success = query.exec("DROP TABLE IF EXISTS records");

    if (!success) {
        qDebug() << "Error: Failed to delete table" << query.lastError().text();
    }
    return success;
}

bool DatabaseManager::insertRecord(const QString &nodeName, const QString &sheetName, const QString &uniqueID, const QString &message,
                                   const QString &signal, const QString &deltaTime, const QString &isSHM,
                                   const QString &cycleTime, const QString &value)
{
    QSqlQuery query;
    query.prepare("INSERT INTO records (NodeName, SheetName, UniqueID, Message, Signal, DeltaTime, IsSHM, CycleTime, Value) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(nodeName);
    query.addBindValue(sheetName);
    query.addBindValue(uniqueID);
    query.addBindValue(message);
    query.addBindValue(signal);
    query.addBindValue(deltaTime);
    query.addBindValue(isSHM);
    query.addBindValue(cycleTime);
    query.addBindValue(value);

    bool success = query.exec();

    if (!success) {
        qDebug() << "Error: Failed to insert record" << query.lastError().text();
    }

    return success;
}

bool DatabaseManager::deleteRecord(const QString &condition)
{
    QSqlQuery query;
    query.prepare("DELETE FROM records WHERE " + condition);
    bool success = query.exec();

    if (!success) {
        qDebug() << "Error: Failed to delete record" << query.lastError().text();
    }

    return success;
}

QSqlQueryModel* DatabaseManager::readData()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM records");

    if (model->lastError().isValid()) {
        qDebug() << "Error: Failed to read data" << model->lastError().text();
    }

    return model;
}

bool DatabaseManager::removeAllRecords()
{
    QSqlQuery query;
    bool success = query.exec("DELETE FROM records");

    if (!success) {
        qDebug() << "Error: Failed to remove all records" << query.lastError().text();
    }

    return success;
}

bool DatabaseManager::isDatabaseOpen()
{
    return db.isOpen();
}
