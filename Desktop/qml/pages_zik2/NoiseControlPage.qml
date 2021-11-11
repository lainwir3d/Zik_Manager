import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import ZikManager 1.0

import "../elements/"
import "../elements/desktop"


Item {
    id: firstPage

    property int baseMargin: 10

   CustomButton {
        id: backButton

        anchors.margins: firstPage.baseMargin
        anchors.top: parent.top
        anchors.left: parent.left

        text: "Back"
        textColor: "white"

        onClicked: {
            popPage();
        }

        function popPage() {
            firstPage.Stack.view.pop();
        }
    }

    focus: true
    Keys.onBackPressed: {
        backButton.popPage();
    }
    Component.onCompleted: {
        forceActiveFocus();
    }


    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: backButton.bottom

        text: zik.externalNoise != -1 ? zik.externalNoise + "dB" : ""
        color: "white"
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        text: zik.internalNoise != -1 ? zik.internalNoise + "dB" : ""
        color: "white"
    }

    LabeledSwitch {
        id: autoNC

        height: firstPage.baseMargin * 2

        visible: zik.capabilities & Zik.AutoNc

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: nc.top
        anchors.bottomMargin: firstPage.baseMargin

        text: qsTr("Auto Noise Control")
        textColor: "white"

        switchCheckedColor: "orange"
        switchUncheckedColor: "white"

        backgroundPressedColor: "black"
        backgroundPressedOpacity: 0.5

        checked: zik.autoNC
        onCheckedChanged: {
            zik.autoNC = checked
        }

        onLabelClicked: {
            checked = zik.autoNC ? false : true;
        }
    }

    NoiseControl{
        id: nc

        width: parent.width < parent.height ? parent.width * 0.90 : height
        height: parent.height < parent.width ? parent.height * 0.60 : width

        anchors.centerIn: parent

        fixedRandomNoiseCoeff: width/4
        handleSize: width/20

        autoNc: zik.capabilities & Zik.AutoNc ? zik.autoNC : false

        rotation: 90

        handlesColor: "white"

        antialiasing: true
    }
}
