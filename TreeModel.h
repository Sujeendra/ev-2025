#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QString>
#include <QDebug>
#include <QDir>
#include "TreeItem.h"
#include <QCanDbcFileParser>
#include <qqml.h>
#include <QCanMessageDescription>
#include <QCanSignalDescription>
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    TreeItem *m_rootItem;
};

#endif // TREEMODEL_H
