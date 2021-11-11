import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0
import "qrc:/elements"

Page {
    id: firstPage

    PageHeader {
        id: hdr
        title: "Noise Control"
    }

    Text {
        id: externalNoise

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: nc.top
        anchors.bottomMargin: Theme.paddingMedium
        font.pixelSize: Theme.fontSizeHuge

        text: zik.externalNoise + "dB"
        color: "white"

        visible: zik.externalNoise == "-1" ? false : true
    }

    TextSwitch{
        id: autoNC

        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.bottom: nc.top
        anchors.topMargin: 50

        visible: zik.capabilities & Zik.AutoNc

        text: qsTr("Auto Noise Control")

        checked: zik.autoNC
        onCheckedChanged: {
            zik.autoNC = checked
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: Theme.fontSizeHuge

        text: zik.internalNoise + "dB"
        color: "white"

        visible: zik.externalNoise == "-1" ? false : true
    }

    NoiseControl{
        id: nc
        width: Math.min(parent.width * 0.90, parent.height * 1/2)
        height: width

        anchors.centerIn: parent

        handlesColor: "white"
        handleSize: width / 15

        rotation: 90

        autoNc: zik.capabilities & Zik.AutoNc ? zik.autoNC : false
    }
}
