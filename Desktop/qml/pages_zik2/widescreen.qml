import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import ZikManager 1.0

import "../elements/"
import "../elements/desktop"

Item {
    id: page

    Component.onCompleted: {
        zik.setAccountUsername("QZik");
    }

    Connections {
        target: zik
        onEqualizerChanged: {
            eq.checked = zik.equalizer == "true" ? true : false;
        }
        onConcertHallChanged: {
            ca.checked =  zik.concertHall == "true" ? true : false;
        }
        onNcEnabledChanged: {
            nc.buttonState = (zik.autoNC && (zik.ncEnabled == "true")) ? 2 : zik.ncEnabled == "true" ? 1 : 0
        }
    }

    Image{
        id: settingsImg

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        width: 30
        height: 30

        source: Qt.resolvedUrl("icon-camera-settings.png")

        MouseArea {
            anchors.fill: parent

            onClicked: {
                page.Stack.view.push({item: Qt.resolvedUrl("SettingsPage.qml")})
            }
        }
    }

    CustomButton {
        id: demoModeBt

        anchors.margins: 10
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        text: "Quit Demo mode"
        textColor: "white"

        visible: zik.demoMode

        onClicked: {
            zik.demoMode = false;
        }
    }

    BatteryStatus{
        id: batteryWidget

        anchors.top: zik.demoMode ? demoModeBt.bottom : parent.top
        anchors.bottom: tools.top

        anchors.margins: 15

        anchors.horizontalCenter: parent.horizontalCenter

        width: height

        percent: zik.battery
        timeleft: -1

        //fontFamily: "DejaVu Sans Mono"
    }



    Row {
        id: tools

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10

        anchors.horizontalCenter: parent.horizontalCenter

        spacing: 30

        Column {
            spacing: 10

            ConcertHall {
                id: concertHall

                width: page.width / 4
                height: width

                circleOpacity: 0.4
                handleOpacity: 0.4
                textVisible: true
                handleDiameter: width/10

                handleImage: "qrc:///elements/icon-m-speaker.png"

                surround: zik.surroundMode
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

            CustomSwitch {
                id:ca
                anchors.horizontalCenter: parent.horizontalCenter

                checkedColor: "white"
                uncheckedColor: "lightgrey"

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

        Column {
            spacing: 10

            NoiseControl{
                width: page.width / 4
                height: width

                antialiasing: true

                fixedRandomNoiseCoeff: width/4
                handleSize: width/20

                rotation: 90

                autoNc: zik.autoNC

                handlesColor: "white"
                borderColor: "black"
            }

            ThreeStateSwitch {
                id: nc
                anchors.horizontalCenter: parent.horizontalCenter

                offColor: "lightgrey"
                onColor: "white"
                thirdStateColor: "white"

                thirdStateText: "AUTO"

                thirdStateEnabled: zik.capabilities & Zik.AutoNc
                buttonState: (zik.autoNC && (zik.ncEnabled == "true")) ? 2 : zik.ncEnabled == "true" ? 1 : 0

                onButtonStateChanged: {
                    if(buttonState == 0){
                        zik.autoNC = false;
                        zik.ncEnabled = "false";
                    }else if(buttonState == 1){
                        zik.autoNC = false;
                        zik.ncEnabled = "true";
                    }else if(buttonState == 2){
                        zik.autoNC = true;
                        zik.ncEnabled = "true";
                    }
                }
            }
        }

        Column {
            spacing: 10

            ThumbEqualizer{
                id: te

                width: page.width / 4
                height: width

                circleOpacity: 0.5
                textOpacity: 1.0

                r: zik.thumb.x
                theta: zik.thumb.y

                fillColor: "lightgrey"
                strokeColor: "gold"
                lineWidth: 1

                handleFillColor: "gold"
                handleStrokeColor: "darkgrey"

                handleRadius: 20
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
                            //console.log("r="+r+"   theta="+theta);
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
                    //console.log("animRunning " + animRunning);
                    if(animRunning && (!pressed)){
                        t1.start();
                    }else if((!animRunning) && (!pressed)){
                        t1.stop();
                        updateThumbEqua();
                    }
                }

                Timer {
                    id: t1
                    interval: 50; running: false; repeat: true
                    onTriggered: {
                        if(parent.r <=100){	// security
                            zik.thumb = Qt.point(parent.r, parent.theta);
                        }
                    }
                }
            }

            CustomSwitch {
                id: eq
                anchors.horizontalCenter: parent.horizontalCenter

                checkedColor: "white"
                uncheckedColor: "lightgrey"

                checked: zik.equalizer == "true" ? true : false
                onCheckedChanged: {
                    if(checked){
                        zik.equalizer = "true"
                    }else{
                        zik.equalizer = "false"
                    }
                }
            }
        }
    }


    Item{
        enabled: false
        visible: false

        id: metapanel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        width:parent.width
        height: parent.height * 1/10

        opacity: !zik.playing ? 0.0 : zik.source != "line-in" ? 1.0 : 0.0

        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.Linear } }

        Connections {
            target: zik
            onSourceChanged: {
                if(zik.source == "a2dp"){
                    console.log("Metapanel: Switch from line-in to A2DP detected. Starting timer.");
                    t_meta_source.start();
                }else{
                    if(t_meta_source.running){
                        console.log("Metapanel: Switched back to line-in ! Canceling timer.");
                        t_meta_source.stop();
                    }
                    metapanel.opacity = 0.0;
                }
            }
            onPlayingChanged: {
                if(zik.playing == false){
                    console.log("Metapanel: Paused detected. Starting timer.");
                    t_meta_playing.start();
                }else{
                    if(t_meta_playing.running){
                        console.log("Metapanel: Playing again ! Canceling timer.");
                        t_meta_playing.stop();
                    }
                    if(zik.source == "a2dp") metapanel.opacity = 1.0;
                }
            }
        }

        Timer {
            id: t_meta_source
            interval: 5000
            onTriggered: {
                if(zik.playing) metapanel.opacity = 1.0;
            }
        }

        Timer {
            id: t_meta_playing
            interval: 3000
            onTriggered: {
                metapanel.opacity = 0.0;
            }
        }

        Rectangle {
            anchors.fill: parent
            z:6

            color: "white"
            opacity: 0.3
        }


        Image{
            anchors.right: parent.right
            anchors.rightMargin: 10

            height: parent.height * 0.9
            width: height

            source: zik.artUrl

        }


        Column{
            z:7
            anchors.fill: parent
            anchors.margins: parent.height * 1/10
            spacing: (height - artistAlbumText.paintedHeight - titleText.paintedHeight) / 2

            Text {
                id: artistAlbumText

                color: "white"

                text: zik.artist + " - " + zik.album
            }

            Text {
                id: titleText

                color: "white"

                text: zik.song
            }
        }
    }
}
