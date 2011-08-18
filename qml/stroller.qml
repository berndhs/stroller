import QtQuick 1.0
import moui.geuzen.geo.static 1.0
import moui.geuzen.utils.static 1.0

Rectangle {
  id: mainBox
  color:  "transparent"
  property int rowHeight: (isProbablyPhone ? 64 : 42)
  property bool bigLetters: isProbablyPhone
  property bool isPortrait: false //isProbablyPhone
  property real mainWidth: isPortrait ? height : width
  property real mainHeight: isPortrait ? width : height
  rotation: 0
  GeuzenGeoTicker {
    id: geoDataSource
    onPositionUpdate: {
      currentDisplay.posLat = posLat
      currentDisplay.posLon = posLon
      currentDisplay.posAlt = posAlt
      currentDisplay.groundSpeed = velHorizontal
      currentDisplay.heading = heading
      currentDisplay.climbRate = velVertical
    }
    onMapUpdate: {
      mapImage.source = fileName
      console.log ("XXXXXXXXXXXXXX new map " + mapImage.source)
    }
  }
  GeuzenOrientation {
    id: orientationWatcher
    onRotationChange: {
      mainBox.isPortrait = portrait
      mainBox.rotation = rotation
      console.log ("new orientation port " + mainBox.isPortrait)
      console.log ("main box x " + mainBox.x + " y " + mainBox.y)
      console.log (" display locations " + currentDisplay.x + " , " + currentDisplay.y)
      console.log (" display size w " + currentDisplay.width + " h " + currentDisplay.height)
      console.log (" buttons location " + buttonBox.x + ", " + buttonBox.y)
    }
  }
  Rectangle {
    id: buttonBox
    color: "white"
    property real extraWidth: 8
    width: mainWidth; height: rowHeight
    property real buttonWidth: width * 0.3
    anchors {
      top: mainBox.top
      horizontalCenter: mainBox.horizontalCenter
    }
    Row {
      spacing: buttonBox.extraWidth
      Rectangle {
        height: rowHeight; width: buttonBox.buttonWidth
        color: "red"
        radius: 0.4*height
        Text {
          anchors.centerIn: parent
          text: qsTr ("Close")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            Qt.quit()
          }
        }
      }
      Rectangle {
        height: rowHeight; width: buttonBox.buttonWidth
        color: "lightgreen"
        radius: 0.4*height
        Text {
          anchors.centerIn: parent
          text: qsTr ("Start")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            geoDataSource.start()
          }
        }
      }
      Rectangle {
        height: rowHeight; width: buttonBox.buttonWidth
        color: "yellow"
        radius: 0.4*height
        Text {
          anchors.centerIn: parent
          text: qsTr ("Stop")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            geoDataSource.stop()
          }
        }
      }
    }
  }

  Rectangle {
    id: currentDisplay
    property real posLat: 0.0
    property real posLon: 0.0
    property real posAlt: 0.0
    property real groundSpeed: 0.0
    property real heading: 0.0
    property real climbRate: 0.0
    width: mainWidth
    height: textDisplay.height
    color: "#e0f0f7"
    anchors {
      top: buttonBox.bottom
      left: buttonBox.left
    }

    Text {
      id:textDisplay
      anchors {
        top: currentDisplay.top;
        left: currentDisplay.left
      }
      width: currentDisplay.width
      wrapMode: Text.WrapAnywhere
      font.pointSize: bigLetters ? 24 : 16
      color:"red"
      text: "Lat/Lon " + currentDisplay.posLat.toFixed (3) +
            " / " + currentDisplay.posLon.toFixed (3) +
            "\nV " + currentDisplay.groundSpeed.toPrecision(3) + " m/s " +
            " hdg " + currentDisplay.heading.toFixed (1) + " deg" +
            "\nAltitude " + currentDisplay.posAlt.toPrecision(3) +
            " climb    " + currentDisplay.climbRate.toPrecision(3) + " m/s"
    }
  }
  Rectangle {
    id:mapBox
    anchors { top: currentDisplay.bottom; horizontalCenter:currentDisplay.horizontalCenter }
    width: mainWidth
    height:mainHeight - currentDisplay.height - buttonBox.height
    color:"blue"
    Image {
      id:mapImage
      width: parent.width - 2
      height: parent.height - 2
      anchors.centerIn:parent
      source: ""
    }
  }

  Component.onCompleted: {
    orientationWatcher.start()
    geoDataSource.setInterval (3000, true)
    console.log (" loaded, width " + mainWidth + " height "+ mainHeight)
    console.log (" color " + mainBox.color)
  }
}
