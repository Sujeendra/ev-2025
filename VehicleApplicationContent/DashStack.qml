
import QtQuick
import QtQuick.Controls
import QtQuick.Timeline
import QtQuick.Studio.Components
import QtQuick.Studio.Effects
import Qt.labs.qmlmodels



Item {
    id: cluster_Art
    width: 720
    height: 400


        Fuel_dial_195_43 {
            id: fuelDial
            x: 8
            y: 35
            fuelFrame: signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_ULvl"]].value || 0

            rangeDisplay: signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_ULvl"]].value.toString() || "0"

            litersDisplay:  signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_I"]].value || "0"

        }
        Speed_dial_195_151Form{
            id: speedDial
            x: 244
            y: 35
           kphFrame : signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_AvlChPwr"]].value || 0

           kphDisplay : signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_AvlChPwr"]].value || "0"

           kplDisplay : signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD1_PGN61584.HS_AvlDchaPwr"]].value || "0"

        }
        HorizontalHeaderView {
            id: horizontalHeader
            x: 540
            width: 162
            height: 33
            anchors.left: tableView.left
            anchors.top: parent.top
            anchors.topMargin: 71
            syncView: tableView1
            clip: true

            // Custom header model with SheetName as the first column
            model: ListModel {
                ListElement { text: "Signal" }
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
            anchors.leftMargin: 540
            anchors.rightMargin: 8
            anchors.topMargin: 106
            anchors.bottomMargin: 140
            columnSpacing: 1
            rowSpacing: 1
            clip: true

            model: TableModel {
                TableModelColumn { display: "Signal" }
                TableModelColumn { display: "Value" }

                rows: [
                    {
                        "Signal": "BMS. MaxI",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["Vehicle_Control_Unit.BMS.BMS_Max_Current"]].value?.toString()+"A" || "0A"

                    },
                    {
                        "Signal": "BMS MaxV",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["Vehicle_Control_Unit.BMS.BMS_Max_Voltage"]].value?.toString()+"V" || "0V"

                    },
                    {
                        "Signal": "Bat. Temp",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSD3_PGN61586.HS_HiCellTemp"]].value?.toString()+"C" || "0C"

                    },
                    {
                        "Signal": "Bat. DisR",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_HiUBusDcnctForewarning"]].description?.toString() || "NA"

                    },
                    {
                        "Signal": "HVIL Sts",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_HVILSts"]].description?.toString() || "NA"

                    },
                    {
                        "Signal": "No. Bat",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.HVESSS1_PGN61590.HS_NrofHPsRdy"]].value?.toString() || "0"

                    },
                    {
                        "Signal": "Chr. Limit",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.PropA_C3_PGN61184.IChLimLong"]].value?.toString()+"A" || "0A"

                    },
                    {
                        "Signal": "Dch. Limit",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["VIC.PropA_C3_PGN61184.UChLimLong"]].value?.toString()+"V"|| "0V"

                    },
                    {
                        "Signal": "OBC Temp",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["OBC.OBC.OBC_Temperature"]].value?.toString()+"C" || "0C"
                    },
                    {
                        "Signal": "OBC I",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["OBC.OBC.OBC_Output_Current"]].value.toString()+"A" || "0A"
                    },
                    {
                        "Signal": "OBC V",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["OBC.OBC.OBC_Output_Voltage"]].value.toString()+"V" || "0V"
                    },
                    {
                        "Signal": "DCDC Temp",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Temperature"]].value.toString()+"C" || "0C"
                    },
                    {
                        "Signal": "DCDC I",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Output_Current"]].value.toString()+"A" || "0A"
                    },
                    {
                        "Signal": "DCDC V",
                        "Value":
                        signalValueMap.signalValueList[gData.signalIndexMap["DCDC.DCDC.DC_Output_Voltage"]].value.toString()+"V" || "0V"
                    }//,
                    // {
                    //     "Signal": "PDU V",
                    //     "Value":
                    //     signalValueMap.signalValueList[gData.signalIndexMap["PDU.PDU_Tx_2.HV_DC_1_U_V"]].value.toString()+"V" || "0V"
                    // },
                    // {
                    //     "Signal": "PDU I",
                    //     "Value":
                    //     signalValueMap.signalValueList[gData.signalIndexMap["PDU.PDU_Tx_2.HV_DC_1_I_A"]].value.toString()+"A" || "0V"
                    // },
                    // {
                    //     "Signal": "Edi. Mot. Spd",
                    //     "Value":
                    //     signalValueMap.signalValueList[gData.signalIndexMap["Editron.ST_MOT_1.st_mot_n"]].value.toString()+" rpm" || "0 rpm"
                    // }

                ]
            }

            delegate: Rectangle {
                implicitWidth: 80
                implicitHeight: 50
                border.width: 1

                Text {
                    text: display
                }

            MouseArea {
                    anchors.fill: parent
                    onClicked: popupText2.open()
                }
            Popup {
                   id: popupText2
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
                           onClicked: popupText2.close()
                       }
                   }
            }
            }
        }


    Timeline {
        id: bootTImeline
        animations: [
            TimelineAnimation {
                id: timelineAnimation
                property: "currentFrame"
                duration: 5000
                running: false
                loops: 1
                to: 5000
                from: 0
            }
        ]
        startFrame: 0
        endFrame: 5000
        enabled: true

        KeyframeGroup {
            target: flipable
            property: "flipAngle"
            Keyframe {
                value: 180
                frame: 0
            }

            Keyframe {
                value: 180
                frame: 2389
            }

            Keyframe {
                easing.bezierCurve: [0.90, 0.03, 0.69, 0.22, 1, 1]
                value: 1.1
                frame: 4117
            }
        }

        KeyframeGroup {
            target: flipable
            property: "opacity"
            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                easing.bezierCurve: [0.17, 0.84, 0.44, 1.00, 1, 1]
                value: 1
                frame: 1015
            }
        }

        KeyframeGroup {
            target: gearbox_195_196
            property: "opacity"

            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 3616
            }
        }

        KeyframeGroup {
            target: rpmDial
            property: "opacity"

            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 2386
            }

            Keyframe {
                easing.bezierCurve: [0.95, 0.05, 0.80, 0.04, 1, 1]
                value: 1
                frame: 3299
            }
        }

        KeyframeGroup {
            target: rpmDial
            property: "scale"
            Keyframe {
                value: 0.01
                frame: 5
            }

            Keyframe {
                value: 0.01
                frame: 2391
            }

            Keyframe {
                easing.bezierCurve: [0.95, 0.05, 0.80, 0.04, 1, 1]
                value: 1
                frame: 3294
            }
        }

        KeyframeGroup {
            target: fuelDial
            property: "opacity"
            Keyframe {
                frame: 0
                value: 0
            }

            Keyframe {
                frame: 2386
                value: 0
            }

            Keyframe {
                frame: 3299
                value: 1
                easing.bezierCurve: [0.95, 0.05, 0.80, 0.04, 1, 1]
            }
        }

        KeyframeGroup {
            target: fuelDial
            property: "scale"
            Keyframe {
                frame: 5
                value: 0.01
            }

            Keyframe {
                frame: 2391
                value: 0.01
            }

            Keyframe {
                frame: 3294
                value: 1
                easing.bezierCurve: [0.95, 0.05, 0.80, 0.04, 1, 1]
            }
        }

        KeyframeGroup {
            target: isoIconsEffect
            property: "scale"

            Keyframe {
                value: 0.01
                frame: 0
            }

            Keyframe {
                value: 0.01
                frame: 2977
            }

            Keyframe {
                easing.bezierCurve: [0.07, 0.82, 0.17, 1.00, 1, 1]
                value: 1
                frame: 3473
            }
        }

        KeyframeGroup {
            target: isoIconsEffect
            property: "opacity"

            Keyframe {
                value: 0
                frame: 2977
            }

            Keyframe {
                easing.bezierCurve: [0.07, 0.82, 0.17, 1.00, 1, 1]
                value: 1
                frame: 3362
            }

            Keyframe {
                value: 0
                frame: 0
            }
        }
    }

    states: [
        State {
            name: "bootState"
            when: true

            PropertyChanges {
                target: bootTImeline
                enabled: true
            }

            PropertyChanges {
                target: timelineAnimation
                running: true
            }
        },
        State {
            name: "running"
            when: !false

            PropertyChanges {
                target: gearbox_195_196
                opacity: 1
            }

            PropertyChanges {
                target: bootTImeline
                currentFrame: 5000
                enabled: true
            }
        }
    ]

}
