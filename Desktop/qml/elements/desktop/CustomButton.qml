import QtQuick 2.0

MouseArea {
    id: cbutton

    width: label.paintedWidth * 1.5
    height: label.paintedHeight * 1.5

    property alias text: label.text
    property alias fontPixelSize: label.font.pixelSize
    property alias textColor: label.color
    property alias rectangleColor: rect.color

    Rectangle {
        id: rect
        anchors.fill: parent

        color: "black"
        radius: parent.height / 3

        opacity: 0.5
    }

    Text {
        id: label
        anchors.centerIn: parent

        color: "white"
    }
}
