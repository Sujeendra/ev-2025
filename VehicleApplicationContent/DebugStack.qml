import QtQuick
import QtQuick.Controls
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

Item {
    width: 720
    height: 400
    // Restart Button
    Button {
        y: -117
        text: "Restart"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 478
        anchors.horizontalCenterOffset: 261
        anchors.horizontalCenter: parent.horizontalCenter
        width: 96
        height: 39

        onClicked: {
            systemManager.restartDevice(); // Calls the C++ function to restart
        }
    }
    Button {
        y: -117
        text: "Shutdown"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 478
        anchors.horizontalCenterOffset: 26
        anchors.horizontalCenter: parent.horizontalCenter
        width: 134
        height: 39

        onClicked: {
            systemManager.shutdownDevice(); // Calls the C++ function to restart
        }
    }
    // Connect to Wi-Fi Button
    Button {
        y: 0
        text: "Connect to Wi-Fi"
        anchors.horizontalCenterOffset: -217
        width: 186
        height: 39
        anchors.horizontalCenter: parent.horizontalCenter

        onClicked: {
            wifiDialog.open(); // Open the Wi-Fi credentials dialog
        }
    }
    ScrollView {
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.topMargin: 48

        TextArea {
            id: logTextArea
            x: -10
            y: -6
            width: 720
            height: 352
            readOnly: true
            wrapMode: TextArea.WrapAtWordBoundaryOrAnywhere
            text: ""
            font.pixelSize: 20
            color: "white"
            horizontalAlignment: Text.AlignLeft
            font.bold: true
            font.family: "Times New Roman"
            font.styleName: "Bold"
        }
    }

    Connections {
        target: logManager
        onNewLogAdded: {
            console.log("New log received:", log);
            logTextArea.text += log + "\n";
        }
    }
    // Wi-Fi Credentials Popup Dialog
   Dialog {
       id: wifiDialog
       title: "Connect to Wi-Fi"
       width: 400
       height: 300

       // Wi-Fi SSID and Password Fields
       Column {
           spacing: 10
           padding: 20

           TextField {
               id: ssidInput
               placeholderText: "Enter SSID"
               width: parent.width * 0.8
           }

           TextField {
               id: passwordInput
               placeholderText: "Enter Password"
               echoMode: TextInput.Password
               width: parent.width * 0.8
           }

           Row {
               spacing: 10
               anchors.horizontalCenter: parent.horizontalCenter

               // Connect Button
               Button {
                   text: "Connect"
                   onClicked: {
                       wifiManager.connectToWifi(ssidInput.text, passwordInput.text);
                       wifiDialog.close(); // Close the dialog after connection attempt
                   }
               }

               // Cancel Button
               Button {
                   text: "Cancel"
                   onClicked: {
                       wifiDialog.close(); // Close the dialog without doing anything
                   }
               }
           }
       }
       InputPanel {
           id: inputPanel1
           y: Qt.inputMethod.visible ? parent.height - inputPanel1.height + 40 : parent.height
           anchors.left: parent.left
           anchors.right: parent.right
       }

   }
   Connections {
          target: wifiManager

          // Update UI on Wi-Fi Connection Success
          onWifiConnected: (ssid, ip) => {
              logTextArea.text += "Wi-Fi Status: Connected to " + ssid + "\n";
              logTextArea.text += "IP Address: " + ip + "\n";
          }

          // Update UI on Connection Failure
          onWifiFailed: {
              logTextArea.text += "Wi-Fi Connection Failed!" + "\n";
              logTextArea.text += "IP Address: Not Assigned" + "\n";
          }
      }
}
