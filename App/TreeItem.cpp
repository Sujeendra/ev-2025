#include "TreeItem.h"

TreeItem::TreeItem(const QString &data, TreeItem *parent)
    : m_data(data), m_parent(parent)
{
}

TreeItem::~TreeItem()
{
    // Delete all child items when the parent item is deleted
    qDeleteAll(m_children);
}

void TreeItem::appendChild(TreeItem *child)
{
    // Add the child item to the list of children
    m_children.append(child);
}

TreeItem *TreeItem::child(int row)
{
    // Return the child at the specified index
    return m_children.value(row);
}

int TreeItem::childCount() const
{
    // Return the number of child items
    return m_children.count();
}

QVariant TreeItem::data() const
{
    // Return the data stored in this tree item
    return m_data;
}

TreeItem *TreeItem::parent()
{
    // Return the parent item
    return m_parent;
}

int TreeItem::row() const
{
    // Return the index of this item within its parent's children
    return m_parent ? m_parent->m_children.indexOf(const_cast<TreeItem *>(this)) : 0;
}
