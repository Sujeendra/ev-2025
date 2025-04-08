import QtQuick
import Qt.labs.qmlmodels
import QtQuick.Controls
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import com.example.db 1.0

Item {
    width: 720
    height: 450
    DatabaseManager {
            id: dbManager
    }
    property var columnNames: ["SheetName", "UniqueID", "Message", "Signal", "DeltaTime", "IsSHM", "CycleTime","Value"]

    // Header for TableView
    HorizontalHeaderView {
        id: horizontalHeader
        anchors.left: tableView.left
        anchors.top: parent.top
        syncView: tableView
        clip: true

        // Custom header model with SheetName as the first column
        model: ListModel {
            ListElement { text: "SheetName" }
            ListElement { text: "UniqueID" }
            ListElement { text: "Message" }
            ListElement { text: "Signal" }
            ListElement { text: "DeltaTime" }
            ListElement { text: "IsSHM" }
            ListElement { text: "CycleTime" }
            ListElement { text: "Value" }
        }

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 40
            border.color: "black"  // Set border color to white
            Text {
                anchors.centerIn: parent
                text: model.text // Use the custom header model's text property
                font.bold: true
                color: "black" // Set text color to white
            }

        }
    }

    // TableView displaying the data
    TableView {
        id: tableView
        x: 8
        y: 38
        width: 704
        height: 338
        clip: true
        anchors.top: horizontalHeader.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        columnSpacing: 1
        rowSpacing: 1
        columnWidthProvider: function (column) {
                   switch (column) {
                       case 0: return 120; // SheetName column width
                       case 1: return 100; // UniqueID column width
                       case 2: return 150; // Message column width
                       case 3: return 120; // Signal column width
                       case 4: return 80;  // DeltaTime column width
                       case 5: return 80;  // IsSHM column width
                       case 6: return 100; // CycleTime column width
                       case 7: return 100; // Value column width
                       default: return 100; // Default column width
                   }
        }
        // TableModel with SheetName and other required columns
        model: dbManager.getTableModel()

        selectionModel: ItemSelectionModel {}

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50
            border.color: "black" // Set border color to white for rows
            Text {
                text: model.display // Dynamically fetch the column data
                color: "black" // Set text color to white
            }

            TableView.editDelegate: TextInput {
                text: model.display // Dynamically fetch and update the column data
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                Component.onCompleted: selectAll()
                TableView.onCommit: {
                    dbManager.updateRecord(model.row, columnNames[model.column], text); // Update the database
                }
            }
        }
    }

    InputPanel {
        id: inputPanel
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height - 55 : parent.height
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
