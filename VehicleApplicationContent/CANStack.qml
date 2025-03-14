import QtQuick
import Qt.labs.qmlmodels
import QtQuick.Controls
import QtGraphs


Item{
    width: 720
    height: 400
    QtObject {
           id: globalData
           property real newY: 0.0  // Initialize newY to 0.0
           property string key: ""
           property real minY: 0.0   // Min value for axisY
           property real maxY: 10.0  // Max value for axisY
           property real tickInterval: 1.0 // Interval between ticks for axisY
       }
    TreeView {
        id: treeView
        width: 436
        height: 299
        anchors.verticalCenterOffset: -50
        anchors.horizontalCenterOffset: -134
        anchors.centerIn: parent
        clip: true


        selectionModel: ItemSelectionModel {}

        // The model needs to be a QAbstractItemModel
        model: treeModel

        delegate: Item {
            implicitWidth: padding + label.x + label.implicitWidth + padding
            implicitHeight: label.implicitHeight * 1.5

            readonly property real indentation: 20
            readonly property real padding: 5

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth
            required property int row
            required property int column
            required property bool current

            // Rotate indicator when expanded by the user
            // (requires TreeView to have a selectionModel)
            property Animation indicatorAnimation: NumberAnimation {
                target: indicator
                property: "rotation"
                from: expanded ? 0 : 90
                to: expanded ? 90 : 0
                duration: 100
                easing.type: Easing.OutQuart
            }
            TableView.onPooled: indicatorAnimation.complete()
            TableView.onReused: if (current) indicatorAnimation.start()
            onExpandedChanged: indicator.rotation = expanded ? 90 : 0

            Rectangle {
                id: background
                anchors.fill: parent
                color: row === treeView.currentRow ? "darkgray" : "black"
                opacity: (treeView.alternatingRows && row % 2 !== 0) ? 0.3 : 0.1
            }

            Label {
                id: indicator
                x: padding + (depth * indentation)
                anchors.verticalCenter: parent.verticalCenter
                visible: isTreeNode && hasChildren
                text: "▶"

                TapHandler {
                    onSingleTapped: {
                        let index = treeView.index(row, column)
                        treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                        treeView.toggleExpanded(row)
                    }
                }
            }

            Label {
                id: label
                x: padding + (isTreeNode ? (depth + 1) * indentation : 0)
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - padding - x
                clip: true
                font.pixelSize: 20 // Increased font size
                color: "white"     // Changed to white for better contrast on black background
                text: model.display
                TapHandler {
                    onSingleTapped: {
                        if(!hasChildren){

                            // Get the current index
                            let currentIndex = treeView.index(row, column);
                            // Retrieve the parent index
                            let parentIndex = treeView.selectionModel.model.parent(currentIndex);

                            // Extract the parent node name before the first '('
                            let parentName = parentIndex.data().split('(')[0].trim();

                            let grandparentIndex = treeView.selectionModel.model.parent(parentIndex);
                            let grandparentName = grandparentIndex.data();

                            let final_key = grandparentName+"."+parentName+"."+model.display.split('(')[0].trim();
                            //clear singal screen if the key got changed to show new data from start
                            if(final_key!==globalData.key)
                                lineSeries2.clear();
                            globalData.key=final_key;


                    }

                    }
                }
            }

        }
    }
    GraphsView {
          anchors.fill: parent
          anchors.margins: 16
          anchors.leftMargin: 450
          anchors.rightMargin: 8
          anchors.topMargin: 13
          anchors.bottomMargin: 133

          theme: GraphsTheme {
              readonly property color c1: "#DBEB00"
              readonly property color c2: "#373F26"
              readonly property color c3: Qt.lighter(c2, 1.5)
              colorScheme: GraphsTheme.ColorScheme.Dark
              seriesColors: ["#2CDE85", "#DBEB00"]
              grid.mainColor: c3
              grid.subColor: c2
              axisX.mainColor: c3
              axisY.mainColor: c3
              axisX.subColor: c2
              axisY.subColor: c2
              axisX.labelTextColor: c1
              axisY.labelTextColor: c1
          }

          // Use TimeAxis for X-axis to represent time
          axisX: ValueAxis {
              min: 0
              max: 5
              tickInterval: 1
              subTickCount: 9
              labelDecimals: 1
          }

          axisY: ValueAxis {
            min: globalData.minY
            max: globalData.maxY
            tickInterval: globalData.tickInterval
            subTickCount: 4
            labelDecimals: 1
          }

          LineSeries {
              id: lineSeries2
              width: 4
          }

          Timer {
              id: updateTimer
              interval: 500 // 20 milliseconds
              running: true
              repeat: true
              onTriggered: {

                  if(globalData.key!=="")
                  {
                      globalData.newY=signalValueMap.signalValueList[gData.signalIndexMap[globalData.key]].value;
                      result.text="Actual Value: "+globalData.newY+" ("+signalValueMap.signalValueList[gData.signalIndexMap[globalData.key]].description+")";

                      // Add the new point to the LineSeries with x = 5
                      lineSeries2.append(6.0, globalData.newY);
                      // Update the min and max Y-values dynamically based on newY
                      globalData.minY = Math.min(globalData.minY, globalData.newY - 1); // Example dynamic min
                      globalData.maxY = Math.max(globalData.maxY, globalData.newY + 1); // Example dynamic max
                      globalData.tickInterval = (globalData.maxY - globalData.minY) / 10; // Adjust tick interval
                      // Check if the series has more than the desired number of points (e.g., 6)
                      if (lineSeries2.count > 6) {
                          // Temporarily store all points and shift their x-values
                          var points = [];
                          for (var i = 1; i < lineSeries2.count; i++) { // Skip the first point (index 0)
                              var point = lineSeries2.at(i);
                              points.push({ x: point.x - 1, y: point.y }); // Decrement x by 1
                          }

                          // Clear the series and re-add the modified points
                          lineSeries2.clear();
                          for (var j = 0; j < points.length;j++) {
                              lineSeries2.append(points[j].x, points[j].y);
                          }
                      }
                  }
              }
          }




    }

    Label {
        id: result
        x: 39
        y: 366
        width: 661
        height: 34
        text: qsTr("Actual Value: 0")
        font.pixelSize: 30
        color: "white" // Ensure the color is explicitly set
        horizontalAlignment: Text.AlignLeft
        font.bold: true
        font.family: "Times New Roman"
        font.styleName: "Bold"
    }






}



