import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0
import "qrc:/elements"

Page {
    id: firstPage

/*
    Label {
        id: angletext
        anchors.margins: 20
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Angle : ") + concertHall.animationAngle
    }
*/

    PageHeader {
        title: "Concert Hall"
    }

    Text {
        id: autoNCText
        text: qsTr("Mode : ")  + (zik.surroundMode ? "surround" : "stereo")
        color: Theme.primaryColor

        visible: zik.capabilities & Zik.ConcertHallSurround

        anchors.margins: 30
        anchors.bottom: concertHall.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ConcertHall {
        id: concertHall
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: Math.min(parent.width*0.90, parent.height * 1/2)
        height: width

        borderColor: Theme.primaryColor
        activeBorderColor: Theme.primaryColor
        fillColor: "transparent"
        activeFillColor: Theme.secondaryHighlightColor
        handlesColor: Theme.highlightColor
        handleDiameter: width * 1/10

        circleOpacity: 0.4
        handleOpacity: 0.4

        surround: zik.surroundMode  // leave it declared before angle ! Important ! Order matters.
        room: zik.roomSize
        angle: zik.spatializerAngle

        handleImage: Qt.resolvedUrl("image://theme/icon-m-speaker")

        onAngleChanged: {
            zik.spatializerAngle = angle;
        }

        onRoomChanged: {
            zik.roomSize = room;
        }
    }
}
