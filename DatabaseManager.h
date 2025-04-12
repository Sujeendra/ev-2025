#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QVariant>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    Q_INVOKABLE QSqlTableModel* getTableModel();
    Q_INVOKABLE void updateRecord(int row, const QString &columnName, const QVariant &newValue);

    bool createTable();
    bool deleteTable();
    bool insertRecord(const QString &nodeName, const QString &sheetName, const QString &uniqueID, const QString &message,
                      const QString &signal, const QString &deltaTime, const QString &isSHM,
                      const QString &cycleTime, const QString &value);
    bool deleteRecord(const QString &condition);
    QSqlQueryModel* readData();
    bool removeAllRecords();
    bool isDatabaseOpen();

private:
    QSqlDatabase db;
    QSqlTableModel *tableModel;
};

#endif // DATABASEMANAGER_H
