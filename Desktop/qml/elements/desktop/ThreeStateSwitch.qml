import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1


Rectangle {
    id: threeStateSwitch

    property int size: 30
    property color offColor: "white"
    property color onColor: "orange"
    property color thirdStateColor: "orange"
    property int buttonState: 0

    property bool thirdStateEnabled: true
    property string thirdStateText: "AUTO"

    implicitWidth: threeStateSwitch.size
    implicitHeight: threeStateSwitch.size
    radius: height/2


    border.width: 1
    border.color: buttonState == 0 ? offColor : buttonState == 1 ? onColor : thirdStateColor

    color: "transparent"

    Text {
        anchors.centerIn: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: buttonState == 0 ? "OFF" : buttonState == 1 ? "ON" : thirdStateText

        color: buttonState == 0 ? offColor : buttonState == 1 ? onColor : thirdStateColor

        font: referenceText.font
    }

    Text {
        id: referenceText
        visible: false
        anchors.centerIn: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: thirdStateText.length > 3 ? thirdStateText : "OFF"    // check if third state text larger than "OFF"

        font.pixelSize: parent.width / 3
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(thirdStateEnabled){
                buttonState = (buttonState == 2) ? 0 : buttonState+1;
            }else{
                buttonState = (buttonState == 1) ? 0 : buttonState+1;
            }
        }
    }
}
