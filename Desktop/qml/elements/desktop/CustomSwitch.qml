import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1


Rectangle {
    id: customSwitch

    property int size: 30
    property color checkedColor: "orange"
    property color uncheckedColor: "white"
    property bool checked: false

    implicitWidth: customSwitch.size
    implicitHeight: customSwitch.size
    radius: height/2


    border.width: 1
    border.color: customSwitch.checked ? customSwitch.checkedColor : customSwitch.uncheckedColor

    color: "transparent"

    Text {
        anchors.centerIn: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: customSwitch.checked ? "ON" : "OFF"

        color: customSwitch.checked ? customSwitch.checkedColor : customSwitch.uncheckedColor

        font: referenceText.font
    }

    Text {
        id: referenceText
        visible: false
        anchors.centerIn: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: "OFF"

        font.pixelSize: parent.width / 3
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            customSwitch.checked = customSwitch.checked ? false : true ;
        }
    }
}

/*
Switch{
    id: customSwitch

    property int size: 30

    property color uncheckedColor: "white"
    property color checkedColor: "orange"



    style: SwitchStyle{

        groove: Rectangle {
            //width: customSwitch.size
            //height: customSwitch.size
            implicitWidth: customSwitch.size
            implicitHeight: customSwitch.size
            radius: implicitWidth/2

            border.width: 1
            border.color: control.checked ? customSwitch.checkedColor : customSwitch.uncheckedColor

            color: "transparent"

            Label {
                anchors.centerIn: parent

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: control.checked ? "ON" : "ON"

                color: control.checked ? customSwitch.checkedColor : customSwitch.uncheckedColor

                width: parent.width - parent.width*0.2
                fontSizeMode: Text.HorizontalFit
                font.pixelSize: parent.width
            }
        }
        handle: Item {

        }
   }

}
*/
