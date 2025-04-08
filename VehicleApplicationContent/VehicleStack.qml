import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels


Item {
    width: 720
    height: 450

    // uncomment this to see the contents
    // Rectangle {
    //     anchors.fill: parent
    //     color: "black" // Background color
    // }

    // Controls section
    HorizontalHeaderView {
        id: horizontalHeader
        x: 439
        width: 284
        height: 33
        anchors.left: tableView.left
        anchors.top: parent.top
        anchors.topMargin: 26
        syncView: tableView1
        clip: true

        // Custom header model with SheetName as the first column
        model: ListModel {
            ListElement { text: "Error Signal" }
            ListElement { text: "Value" }
        }

        delegate: Rectangle {
            implicitWidth: 80
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
    TableView {
        id: tableView1
        anchors.fill: parent
        anchors.leftMargin: 439
        anchors.rightMargin: -24
        anchors.topMargin: 60
        anchors.bottomMargin: 111
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: TableModel {
            TableModelColumn { display: "Error Signal" }
            TableModelColumn { display: "Value" }

            rows: [
                {
                    "Error Signal": "Bat. ErrLvl",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_ULvl"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "HV Req.",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_HiUBusCnctnSts"]].description?.toString() || "NA"

                },
                {
                    "Error Signal": "Bat. Op. Status",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_OperationalSts"]]?.description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "OBC Cha. State",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["OBC.OBC.OBC_Charger_Ready"]]?.description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "OBC CAN Sts",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["OBC.OBC.OBC_Comm_Fault"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "DCDC Err. CAN",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Error_Communication"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "DCDC IU",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Error_Input_Overvoltage"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "DCDC OI",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Error_Output_Overcurrent"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "DCDC OU",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Error_Output_Overvoltage"]].description?.toString() || "Unknown"

                },
                {
                    "Error Signal": "DCDC OStatus",
                    "Value":
                    signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Output_Status"]].description?.toString() || "Unknown"
                }//,
                // {
                //     "Error Signal": "PDU Status",
                //     "Value":
                //     signalValueMap.signalValueList[gData.signalIndexMap["PDU.PDU_Tx_1.byPDU_Status"]].description?.toString() || "Unknown"
                // },
                // {
                //     "Error Signal": "PDU CAN",
                //     "Value": (() => {
                //         let faultStatus = signalValueMap.signalValueList[gData.signalIndexMap["PDU.PDU_Tx_1.CANCommunicationFault"]].value;
                //         return faultStatus === 1 ? "Fault" : faultStatus === 0 ? "Normal" : "Unknown";
                //     })()
                // },
                // {
                //     "Error Signal": "Editron Status",
                //     "Value":
                //     signalValueMap.signalValueList[gData.signalIndexMap["Editron.ST_MOT_2.st_mot_fault"]].description?.toString() || "Unknown"
                // },
                // {
                //     "Error Signal": "Editron Run",
                //     "Value":
                //     signalValueMap.signalValueList[gData.signalIndexMap["Editron.ST_MOT_2.st_mot_run"]].description?.toString() || "Unknown"
                // }
            ]
        }

        delegate: Rectangle {
            implicitWidth: 140
            implicitHeight: 50
            border.width: 1
            color: display.indexOf("Normal") !== -1 ? "green" :
                                                     (display.indexOf("Fault") !== -1 || display.indexOf("Error") !== -1) ? "red" : "white"
            Text {
                text: display
            }
            MouseArea {
                    anchors.fill: parent
                    onClicked: popupText.open()
                }
            Popup {
                   id: popupText
                   width: 700
                   height: 300
                   modal: true
                   focus: true
                   parent: Overlay.overlay

                   x: Math.round((parent.width - width) / 2)
                   y: Math.round((parent.height - height) / 2)
                   Rectangle {
                       anchors.fill: parent
                       color: "lightgray"
                       border.color: "black"

                       Text {
                           text: display
                           horizontalAlignment: Text.AlignHCenter
                           wrapMode: Text.WordWrap
                           width: 690
                       }
                       Button {
                           text: "Close"
                           anchors.bottom: parent.bottom
                           anchors.horizontalCenter: parent.horizontalCenter
                           onClicked: popupText.close()
                       }
                   }


            }

        }


    }

    Rectangle {
        id: controlsSection
        width: parent.width
        height: 250
        color: "transparent"
        anchors.top: parent.top
        anchors.topMargin: 20

        // Controls Section Title
        Text {
            anchors.top: parent.top
            font.pixelSize: 24
            font.bold: true
            color: "white"
            text: "Controls"
            anchors.left: parent.left
            anchors.leftMargin: 20
        }

        // Controls content
        Column {
            anchors.top: parent.top
            anchors.topMargin: 40
            spacing: 20
            anchors.left: parent.left
            anchors.leftMargin: 20

            // Start/Stop button (using Switch)
            Row {
                spacing: 10
                Text {
                    text: "Start/Stop"
                    color: "white"
                    font.pixelSize: 18
                }
                Switch {
                    width: 60
                    id:startstop
                    onCheckedChanged: {
                        mainController.toggleStartStop(checked);
                        if(checked){
                            accharging.enabled = true;
                            dccharging.enabled = true;
                            driving.enabled = true;
                            // crash.enabled = true;
                        }
                        else{
                            accharging.enabled = false;
                            dccharging.enabled = false;
                            driving.enabled = false;
                            // crash.enabled = false;
                        }
                    }
                }
            }

            // Connect Driving button (using Switch)
            Row {
                spacing: 10
                Text {
                    text: "Connect Driving"
                    color: "white"
                    font.pixelSize: 18
                }
                Switch {
                    width: 60
                    id:driving
                    enabled: false
                    onCheckedChanged: {
                        mainController.toggleConnectDriving(checked);
                        if(!checked){
                            accharging.enabled = true;
                            dccharging.enabled = true;
                            // crash.enabled = true;
                        }
                        else{
                            accharging.enabled = false;
                            dccharging.enabled = false;
                            // crash.enabled = false;
                        }
                    }
                }
            }

            // Connect/Disconnect AC Charging button (using Switch)
            Row {
                spacing: 10
                Text {
                    text: "Connect/Disconnect AC Charging"
                    color: "white"
                    font.pixelSize: 18
                }
                Switch {
                    width: 60
                    id:accharging
                    enabled: false
                    onCheckedChanged: {
                        mainController.toggleACCharging(checked);
                        if(!checked){
                            driving.enabled = true;
                            dccharging.enabled = true;
                            // crash.enabled = true;
                        }
                        else{
                            driving.enabled = false;
                            dccharging.enabled = false;
                            // crash.enabled = false;
                        }
                    }
                }
            }

            // Connect/Disconnect DC Charging button (using Switch)
            Row {
                spacing: 10
                Text {
                    text: "Connect/Disconnect DC Charging"
                    color: "white"
                    font.pixelSize: 18
                }
                Switch {
                    width: 60
                    id:dccharging
                    enabled: false
                    onCheckedChanged: {
                         mainController.toggleDCCharging(checked);
                        if(!checked){
                            accharging.enabled = true;
                            driving.enabled = true;
                            // crash.enabled = true;
                        }
                        else{
                            accharging.enabled = false;
                            driving.enabled = false;
                            // crash.enabled = false;
                        }
                    }
                }
            }
        }
    }
    // Separator Line between Controls and Crash Report sections
    Rectangle {
        width: 440
        height: 2
        color: "white" // Line color
        anchors.top: controlsSection.bottom
        anchors.topMargin: 10
    }
    // Crash Report section
    Rectangle {
        id: crashReportSection
        width: parent.width
        height: 80
        color: "transparent"
        anchors.top: controlsSection.bottom // Fix the positioning
        anchors.topMargin: 20 // Add spacing between sections

        // Crash Report Title
        Text {
            anchors.top: parent.top
            font.pixelSize: 24
            font.bold: true
            color: "white"
            text: "Crash Report"
            anchors.left: parent.left
            anchors.leftMargin: 20
        }

        // Crash Report content
        Column {
            anchors.top: parent.top
            anchors.topMargin: 40
            spacing: 20
            anchors.left: parent.left
            anchors.leftMargin: 20

            // Enable Crash Report toggle (using Switch)
            Row {
                spacing: 10
                Text {
                    text: "Enable Crash Report"
                    color: "white"
                    font.pixelSize: 18
                }
                Switch {
                    width: 60
                    id:crash
                    enabled: false
                    onCheckedChanged: {
                        mainController.toggleCrash(checked);
                        if(!checked){
                            accharging.enabled = true;
                            dccharging.enabled = true;
                            driving.enabled = true;
                        }
                        else{
                            accharging.enabled = false;
                            dccharging.enabled = false;
                            driving.enabled = false;
                        }
                    }
                }
            }


        }
    }


}
