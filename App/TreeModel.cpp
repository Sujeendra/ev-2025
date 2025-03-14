#include "TreeModel.h"


TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    // Root item
    m_rootItem = new TreeItem("Root");

    // Parse DBC file
    QCanDbcFileParser fileParser;
    QString path = "/root/vehicle.dbc";
    QString str = QDir::toNativeSeparators(path); // Converts to correct format

    const bool result = fileParser.parse(str);
    if (result) {
        qDebug() << "DBC File Parsed Successfully:" << path;
    } else {
        qDebug() << "Failed Parsing. Error:" << fileParser.errorString();
    }

    // Loop through all messages and populate tree structure
    for (const auto &message : fileParser.messageDescriptions()) {
        QString transmitterName = message.transmitter();
        TreeItem *transmitterNode = nullptr;

        // Check if the transmitter already exists in the tree
        for (int i = 0; i < m_rootItem->childCount(); ++i) {
            if (m_rootItem->child(i)->data() == transmitterName) {
                transmitterNode = m_rootItem->child(i);
                break;
            }
        }

        // If transmitter doesn't exist, create it
        if (!transmitterNode) {
            transmitterNode = new TreeItem(transmitterName, m_rootItem);
            m_rootItem->appendChild(transmitterNode);
        }

        // Prepare message display text
        QString messageName = message.name();
        QString messageComment = message.comment();
        quint32 uniqueId = static_cast<quint32>(message.uniqueId());
        QString messageIdHex = QString("%1").arg(QString::number(uniqueId, 16).toUpper(), 8, QLatin1Char('0')); // 32-bit hex
        QString messageDisplay = QString("%1 (%2) (%3)").arg(messageName).arg(messageIdHex).arg(messageComment);

        // Add message as a child of the transmitter
        TreeItem *messageNode = new TreeItem(messageDisplay, transmitterNode);
        transmitterNode->appendChild(messageNode);

        // Add signals as children of the message
        for (const auto &signal : message.signalDescriptions()) {
            QString signalName = signal.name();
            QString signalComment = signal.comment();
            QString signalDisplay = QString("%1 (%2)").arg(signalName).arg(signalComment);

            // Add the signal as a child of the message
            TreeItem *signalNode = new TreeItem(signalDisplay, messageNode);
            messageNode->appendChild(signalNode);
        }
    }
}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer()) : m_rootItem;
    TreeItem *childItem = parentItem->child(row);
    return childItem ? createIndex(row, column, childItem) : QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer()) : m_rootItem;
    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    return item->data();
}
