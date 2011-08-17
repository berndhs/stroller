import QtQuick 1.0
import moui.geuzen.geo.static 1.0
import moui.geuzen.utils.static 1.0

Rectangle {
  id: mainBox
  color:  "transparent"
  property int rowHeight: (isProbablyPhone ? 64 : 42)
  property bool isPortrait: isProbablyPhone
  property real mainWidth: isPortrait ? height : width
  property real mainHeight: isPortrait ? width : height
  rotation: isPortrait ? -90 : 0
  GeuzenGeoTicker {
    id: tickerSource
    onPositionUpdate: {
      currentDisplay.posLat = posLat
      currentDisplay.posLon = posLon
      currentDisplay.posAlt = posAlt
      currentDisplay.groundSpeed = velHorizontal
      currentDisplay.climbRate = velVertical
    }
  }
  GeuzenOrientation {
    id: orientationWatcher
    onRotationChange: {
      mainBox.isPortrait = portrait
      mainBox.rotation = rotation
    }
  }
  Rectangle {
    id: buttonBox
    color: "white"
    width: mainWidth; height: childrenRect.height
    anchors {
      top: mainBox.top
      horizontalCenter: mainBox.horizontalCenter
    }
    Row {
      spacing: 4
      Rectangle {
        height: rowHeight; width: childrenRect.width
        color: "cyan"
        radius: 0.4*height
        Text {
          anchors.centerIn: parent
          text: qsTr ("  Close  ")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            Qt.quit()
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
    property real climbRate: 0.0
    width: mainWidth
    height: mainHeight - buttonBox.height
    color: "#e0f0f7"
    anchors {
      top: buttonBox.bottom
      horizontalCenter: mainBox.horizontalCenter
    }

    Text {
      anchors {
        top: currentDisplay.top;
        topMargin: rowHeight;
        horizontalCenter: currentDisplay.horizontalCenter
      }
      wrapMode: Text.WrapAnywhere
      font.pointSize: 30
      color:"red"
      text: "Lat " + currentDisplay.posLat +
            " Lon " + currentDisplay.posLon +
            "\n V " + currentDisplay.groundSpeed + " m/s " +
            "\n Altitude " + currentDisplay.posAlt +
            "\n climb    " + currentDisplay.climbRate + " m/s"
    }
  }

  Component.onCompleted: {
    orientationWatcher.start()
    tickerSource.start()
    console.log (" loaded, width " + mainWidth + " height "+ mainHeight)
    console.log (" color " + mainBox.color)
  }
}
