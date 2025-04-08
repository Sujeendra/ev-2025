import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels

Item {
    width: 720
    height: 600

    // Background
    Image {
        id: background
        anchors.fill: parent
        source: "images/back_195_184.png"
    }

    // CST Date and Time Display Container
    Rectangle {
        id: dateTimeContainer
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.horizontalCenterOffset: 1
        radius: 10
        color: "#333333" // Dark background
        width: 718
        height: 50
        border.color: "#ffffff" // White border
        border.width: 1

        // Shadow effect (using multiple rectangles to simulate shadow)
        Rectangle {
            anchors.fill: parent
            color: "black"
            radius: 10
            opacity: 0.3
            width: parent.width
            height: parent.height
            anchors.topMargin: 5
            anchors.leftMargin: 5
        }

        // Date and Time Text
        Text {
            id: dateTimeDisplay
            anchors.centerIn: parent
            font.pixelSize: 18
            font.bold: true
            color: "#ffffff" // White text
            text: timeManager.currentTime // Bind to the C++ TimeManager
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenterOffset: 0
            anchors.horizontalCenterOffset: -223
        }
    }
    // Battery Icon and Percentage Display
    BatteryDisplay {
        id: batteryDisplay
    }
    // Menu Layout
    RowLayout {
        id: top_menu
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: dateTimeContainer.bottom
        anchors.topMargin: -8
        spacing: 8.5

        // Your custom buttons
        Vehicle {
            id: customButton0
            width: 120
            checked: true
            autoExclusive: true
            onClicked: stackLayout.currentIndex = 0
        }

        Dashboard {
            id: customButton1
            width: 120
            autoExclusive: true
            onClicked: stackLayout.currentIndex = 1
        }

        ECUSetup {
            id: customButton2
            width: 120
            autoExclusive: true
            onClicked: stackLayout.currentIndex = 2
        }

        CANData {
            id: customButton3
            width: 120
            autoExclusive: true
            onClicked: stackLayout.currentIndex = 3
        }

        Debug {
            id: customButton4
            width: 120
            autoExclusive: true
            onClicked: stackLayout.currentIndex = 4
        }
    }

    StackLayout {
        id: stackLayout
        x: 0
        y: 129
        width: 720
        height: 509

        VehicleStack {
            id: effectStack1
        }

        DashStack {
            id: effectStack2
        }

        ECUStack {
            id: effectStack3
        }

        CANStack {
            id: effectStack4
        }

        DebugStack {
            id: effectStack5
        }
    }
}
