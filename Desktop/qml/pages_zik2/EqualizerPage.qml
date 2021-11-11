import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import "../elements"
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
     Component.onCompleted: {
         forceActiveFocus();
     }


    ThumbEqualizer{
        id: te
        width: parent.width < parent.height ? parent.width * 2/3 : height
        height: parent.height < parent.width ? parent.height * 2/3 : width
        anchors.centerIn: parent
        circleOpacity: 0.5
        textOpacity: 1.0

        r: zik.thumb.x
        theta: zik.thumb.y

        fillColor: "lightgrey"
        strokeColor: "gold"
        lineWidth: 1

        handleFillColor: "gold"
        handleStrokeColor: "darkgrey"

        handleRadius: width / 10
        handleOpacity: 0.5

        RadialGradient {
            anchors.fill: parent
            //source: parent

            gradient: Gradient {
                GradientStop { position: 0.0; color: "black" }
                GradientStop { position: 0.5; color: "transparent" }
            }
        }

        function updateThumbEqua() {

            if( (!pressed) && (!animRunning) ){
                if(r<=100 && r>=0){	// security
                    zik.thumb = Qt.point(r, theta);
                }
            }else{

            }
        }
/*
        onRChanged: {
            updateThumbEqua();
        }

        onThetaChanged: {
            updateThumbEqua();
        }
*/

        onPressedChanged: {
            updateThumbEqua();

            if(pressed && (!animRunning)){
                t1.start();
            }else if(!animRunning){
                t1.stop();
            }
        }

        onAnimRunningChanged: {
            if(animRunning && (!pressed)){
                t1.start();
            }else if((!animRunning) && (!pressed)){
                t1.stop();
                updateThumbEqua();
            }
        }

        Timer {
            id: t1
            interval: 150; running: false; repeat: true
            onTriggered: {
                if(parent.r <=100){	// security
                    zik.thumb = Qt.point(parent.r, parent.theta);
                }
            }
        }
    }

}
