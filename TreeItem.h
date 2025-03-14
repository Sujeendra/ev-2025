#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QString>

class TreeItem
{
public:
    explicit TreeItem(const QString &data, TreeItem *parent = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);
    TreeItem *child(int row);
    int childCount() const;
    QVariant data() const;
    TreeItem *parent();
    int row() const;

private:
    QString m_data;
    QList<TreeItem *> m_children;
    TreeItem *m_parent;
};

#endif // TREEITEM_H
