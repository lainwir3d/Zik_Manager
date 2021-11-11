import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

MouseArea {
    id: labeledSwitch

    property string text: ""
    property color textColor: "white"

    property bool checked: false

    property color switchUncheckedColor: "white"
    property color switchCheckedColor: "orange"

    property color backgroundReleasedColor: "transparent"
    property color backgroundPressedColor: "transparent"
    property real backgroundReleasedOpacity: 1.0
    property real backgroundPressedOpacity: 1.0

    property string iconSource: ""
    property color iconColor: "white"
    property real iconOpacity: 1.0
    property int iconMargin: 0
    property int iconLeftMargin: 0

    property alias font: label.font

    property int baseMargin: 5

    width: sw.width + label.paintedWidth + baseMargin * 2

    onCheckedChanged: {
        sw.checked = checked;
    }

    signal labelClicked

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

    CustomSwitch{
        z: 1
        id: sw

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: labeledSwitch.iconLeftMargin + labeledSwitch.iconMargin
        anchors.rightMargin: labeledSwitch.iconMargin
        anchors.topMargin: labeledSwitch.iconMargin
        anchors.bottomMargin: labeledSwitch.iconMargin

        width: height

        checkedColor: labeledSwitch.switchCheckedColor
        uncheckedColor: labeledSwitch.switchUncheckedColor

        checked: parent.checked
        onCheckedChanged: {
            labeledSwitch.checked = checked;
        }


    }

    Label{
        z: 1
        id: label

        text: parent.text
        color: parent.textColor

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: sw.right
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


        height: sw.height / 2
        width: height

        source: icon
        color: parent.iconColor

        visible: icon.source != ""

        opacity: parent.iconOpacity
    }

}
