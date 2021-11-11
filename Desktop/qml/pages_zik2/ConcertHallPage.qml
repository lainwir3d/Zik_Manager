import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import ZikManager 1.0

import "../elements"
import "../elements/desktop"

Item {
    id: firstPage

    property int baseMargin: 10
    property int producerMode: 0

    Component.onDestruction: {
        if(producerMode == 1){
            zik.producerMode = false;
        }
    }

    Component.onCompleted: {
        forceActiveFocus();

        if(producerMode == 1){
            loader.sourceComponent = nextPage;
            loader2.sourceComponent = spatia;
            loader3.sourceComponent = enabledSw;

            zik.producerMode = true;
        }else{
            zik.producerMode = false;
        }
    }

    CustomButton {
         id: backButton

         anchors.margins: firstPage.baseMargin
         anchors.top: parent.top
         anchors.left: parent.left

         text: "Back"
         textColor: "white"

         onClicked: {
             if(producerMode == 1){
                 zik.cancelProducerPreset();
                 zik.producerMode = false;
             }

             firstPage.Stack.view.pop();
         }

         function popPage() {
             firstPage.Stack.view.pop();
         }
     }

     focus: true
     Keys.onBackPressed: {
         backButton.popPage();
     }

    Loader {
        id: loader
        anchors.margins: firstPage.baseMargin
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Loader {
        id: loader2
        anchors.margins: firstPage.baseMargin
        anchors.left: parent.left
        anchors.top: backButton.bottom
    }

    Component{
        id: nextPage

        CustomButton {
             id: fwdButton

             text: "Next"
             textColor: "white"

             onClicked: {
                 firstPage.Stack.view.push(Qt.resolvedUrl("ProducerEqualizerPage.qml"));
             }
        }
    }

    Component{
        id: spatia

        LabeledSwitch {
            id: eq

            text: "Bypass"
            textColor: "white"

            switchCheckedColor: "orange"
            switchUncheckedColor: "white"

            height: 40

            checked: zik.presetBypass == "true" ? true : false
            onCheckedChanged: {
                if(checked){
                    zik.presetBypass = "true"
                }else{
                    zik.presetBypass = "false"
                }
            }
        }
    }

    Text {
        id: angletext
        anchors.margins: firstPage.baseMargin
        anchors.top: concertHall.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Angle : ") + concertHall.animationAngle
        color: "white"
    }

    Text {
        id: modetext
        anchors.margins: firstPage.baseMargin
        anchors.bottom: concertHall.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Mode : ") + (zik.surroundMode ? "surround" : "stereo")
        color: "white"

        visible: zik.capabilities & Zik.ConcertHallSurround
    }

    ConcertHall {
        id: concertHall
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.margins: parent.height*0.1
        width: parent.width < parent.height ? parent.width * 0.80 : height
        height: parent.height < parent.width ? parent.height * 0.70 : width

        circleOpacity: 0.4
        handleOpacity: 0.4
        textVisible: true
        handleDiameter: width/10

        handleImage: "qrc:///elements/icon-m-speaker.png"

        surround: zik.surroundMode  // leave it declared before angle ! Important ! Order matters.
        room: zik.roomSize
        angle: zik.spatializerAngle

        currentCircleWidth: 120

        borderColor: "lightgrey"
        activeBorderColor: "white"
        handlesColor: "sandybrown"
        handlesDisabledColor: "grey"
        superposeColors: false

        onAngleChanged: {
            zik.spatializerAngle = angle;
        }

        onRoomChanged: {
            zik.roomSize = room;
        }
    }

    Loader{
        id: loader3
        anchors.bottom: parent.bottom
        anchors.bottomMargin: firstPage.baseMargin * 2
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Component {
        id: enabledSw

        CustomSwitch {
            checked: zik.concertHall == "true" ? true : false
            onCheckedChanged: {
                if(checked){
                    zik.concertHall = "true"
                }else{
                    zik.concertHall = "false"
                }
            }
        }

    }
}
