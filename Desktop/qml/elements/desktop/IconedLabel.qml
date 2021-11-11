import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

MouseArea {
    id: labeledSwitch

    property string text: ""
    property color textColor: "white"
    property real labelOpacity: 1

    property color backgroundReleasedColor: "transparent"
    property color backgroundPressedColor: "transparent"
    property real backgroundReleasedOpacity: 1.0
    property real backgroundPressedOpacity: 1.0

    property bool highlightOnHovered: false
    hoverEnabled: highlightOnHovered

    property string iconSource: ""
    property color iconColor: "white"
    property real iconOpacity: 1.0

    property int baseMargin: 5

    signal labelClicked

    property alias labelPaintedWidth: label.paintedWidth

    onClicked: {
        labelClicked();
    }
    onPressed: {
        bg.color = backgroundPressedColor;
        bg.opacity = backgroundPressedOpacity;
    }
    onReleased: {
        bg.color = backgroundReleasedColor;
        bg.opacity = backgroundReleasedOpacity;
    }
    onEntered: {
        bganimExited.stop();
        bganimEntered.start();
    }
    onExited: {
        bganimEntered.stop();
        bganimExited.start();
    }

    ParallelAnimation {
        id: bganimEntered
        ColorAnimation {
            target: bg
            property: "color"

            from: bg.color
            to: backgroundPressedColor

            duration: 200
        }

        NumberAnimation {
            target: bg
            property: "opacity"

            from: bg.opacity
            to: backgroundPressedOpacity

            duration: 200
            easing.type: Easing.Linear
        }
    }

    ParallelAnimation {
        id: bganimExited
        ColorAnimation {
            target: bg
            property: "color"

            from: bg.color
            to: backgroundReleasedColor

            duration: 200
        }

        NumberAnimation {
            target: bg
            property: "opacity"

            from: bg.opacity
            to: backgroundReleasedOpacity

            duration: 200
            easing.type: Easing.Linear
        }
    }

    Label{
        z: 1
        id: label

        text: parent.text
        color: parent.textColor
        opacity: parent.labelOpacity

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.margins: labeledSwitch.baseMargin
    }

    Rectangle {
        z: 0
        id: bg

        anchors.fill: parent
        color: backgroundReleasedColor
    }

    Image {
        id: icon
        source: parent.iconSource
        mipmap: true
        sourceSize.width: 500
        sourceSize.height: 500

        visible: false
    }

    ColorOverlay {
        id: overlay

        anchors.right: parent.right
        anchors.margins: labeledSwitch.baseMargin

        anchors.verticalCenter: parent.verticalCenter


        height: (bg.height-10) / 2
        width: height

        source: icon
        color: parent.iconColor

        visible: icon.source != ""

        opacity: parent.iconOpacity
    }

}
