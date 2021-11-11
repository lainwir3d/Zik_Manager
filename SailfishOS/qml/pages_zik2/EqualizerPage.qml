import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import "qrc:/elements"

Page {
    id: firstPage


    PageHeader {
        title: "Equalizer"
    }

    ThumbEqualizer{
        width: Math.min(parent.width*0.80, parent.height * 1/2)
        height: width
        anchors.centerIn: parent

        circleOpacity: 0.5
        textOpacity: 1.0

        fillColor: Theme.secondaryHighlightColor
        strokeColor: Theme.primaryColor

        handleFillColor: Theme.highlightColor
        handleStrokeColor: Theme.primaryColor

        handleRadius: width * 1/10
        handleOpacity: 0.5

        r: zik.thumb.x
        theta: zik.thumb.y

        function updateThumbEqua() {

            if( (!pressed) && (!animRunning) ){
                if(r <=100 && r>=0){	// security
                    zik.thumb = Qt.point(r, theta);
                }
            }else{

            }
        }

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

        RadialGradient {
            id: gradient
            anchors.fill: parent

            gradient: Gradient {
                GradientStop { position: 0.0; color: "black" }
                GradientStop { position: 0.5; color: "transparent" }
            }
        }
    }

}
