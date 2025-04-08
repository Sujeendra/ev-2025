import QtQuick

Row {
    id: batteryDisplay
    anchors.top: parent.top
    anchors.right: parent.right
    anchors.topMargin: 20
    anchors.rightMargin: 20
    spacing: 4

    // Battery icon
    Image {
        id: batteryIcon
        width: 30
        height: 30
        source: "images/battery_icon.jpg" // Replace with your battery icon path
        rotation: -90
    }

    // Battery percentage
    Text {
        id: batteryPercentage
        font.pixelSize: 18
        font.bold: true
        color: "white"
        text: signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD2_PGN61585.HS_FastUpdSOC"]].value +"%" || "0%"

    }

    Text {
        id: charging
        font.pixelSize: 18
        font.bold: true
        color: "white"
        text: signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_IntChrgrSts"]].description  || "Charging Off"

    }
    Rectangle {
        width: 2
        height: 30
        color: "gray"
    }
    Text {
        id: soh
        font.pixelSize: 18
        font.bold: true
        color: "white"
        text: "soh - "+signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSHIST_PGN64606.HS_StofHealth"]].value +"%" || "0%"
    }
}
