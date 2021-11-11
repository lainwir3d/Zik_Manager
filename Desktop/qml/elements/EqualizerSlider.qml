import QtQuick 2.0

import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.2

Item{
    id: equaSlider


    property real gain: 0
    property int frequency
    property real q: 1


    Slider {
        id: gainSlider

        anchors.top: parent.top
        anchors.left: parent.left
        orientation: Qt.Vertical

        minimumValue: -12.0
        maximumValue: 12.0
        stepSize: 0.25

        onPressedChanged: {
            if(!pressed){
                equaSlider.gain = value;
            }
        }

        /*onValueChanged: {
            equaSlider.gain = value;
        }*/

        style: SliderStyle {
            groove: Rectangle {
                id: gainGroove
                implicitWidth: 250
                implicitHeight: 4
                color: "black"
                radius: 12
            }
            handle: Rectangle {
                id: gainHandle
                //anchors.centerIn: parent
                y: parent.width / 2 - 15
                color: control.pressed ? "white" : "lightgray"
                border.color: "black"
                border.width: 2
                width: 17
                height: 28
                radius: 6
                Text{
                    anchors.bottom: parent.top
                    text: gainSlider.value + "dB"
                    font.pixelSize: 13
                }
            }
        }
    }

    Slider {
        id: qSlider

        z: 1

        anchors.top: parent.top
        x: gainSlider.x + 10

        orientation: Qt.Vertical

        minimumValue: 0.4
        maximumValue: 4.0
        stepSize: 0.1

        value: 1

        onPressedChanged: {
            if(!pressed){
                equaSlider.q = value;
            }
        }

        /*onValueChanged: {
            equaSlider.q = value;
        }*/


        style: SliderStyle {
            groove: Rectangle {
                implicitWidth: 250
                implicitHeight: 4
                color: "black"
                radius: 12
                opacity: 0
            }
            handle: Rectangle {
                anchors.centerIn: parent
                color: control.pressed ? "white" : "lightgray"
                border.color: "black"
                border.width: 2
                width: 17
                height: 25
                radius: 6
                Text{
                    anchors.top: parent.bottom
                    text: "q:" + String(qSlider.value).substring(0,3)
                    font.pixelSize: 13
                }
            }
        }
    }

}
