import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import ZikManager 1.0
import "qrc:/elements"
import "../"

Page {
    id: page

    allowedOrientations: Orientation.LandscapeMask

    Connections {
        target: zik
        onDisconnected: {
            pageStack.push(Qt.resolvedUrl("../LoadingPage.qml"));
        }
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView

        PullDownMenu {
            MenuItem {
                text: "Quit demo mode"
                onClicked: zik.demoMode = false;
                visible: zik.demoMode
            }

            MenuItem {
                text: "Settings"
                onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
            }
            /*
        MenuItem {
        text: "Presets"
        onClicked: pageStack.push(Qt.resolvedUrl("PresetPage.qml"))
        }
        */
        }

        Connections {
            target: zik
            onEqualizerChanged: {
                eqSwitch.checked = zik.equalizer == "true" ? true : false;
            }
            onNcEnabledChanged: {
                ncSwitch.checked = zik.ncEnabled == "true" ? true : false;
            }
            onConcertHallChanged: {
                caSwitch.checked =  zik.concertHall == "true" ? true : false;
            }
        }

        PageHeader {
            id: header
            title: zik.demoMode ? "Zik 3 Manager (demo mode)" : zik.model == "ZIK3" ? "Zik 3 Manager" : "Zik 2.0 Manager"
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: height

        BatteryStatus{
            id: battery

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.bottom: widgetsRow.top
            anchors.margins: Theme.paddingLarge * 2

            width: height

            percent: zik.battery
            timeleft: -1

            fontFamily: Theme.fontFamily
            textColor: Theme.primaryColor
        }

        Row {
            id: widgetsRow

            height: Math.max(ncRow, caRow, eqRow)

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: Theme.paddingSmall

            spacing: Theme.paddingLarge * 2

            property int elementNb: 3
            property int elementWidth: (parent.width - (spacing * (elementNb+1)) ) / elementNb
            property int elementHeight: height

            Column {
                id: ncRow

                width: parent.elementWidth
                height: width + ncSwitch.height

                NoiseControl{
                    width: parent.width
                    height: width

                    handlesColor: "white"
                    handleSize: width / 15

                    rotation: 90

                    autoNc: zik.autoNC

                    anchors.horizontalCenter: parent.horizontalCenter

                    Label {
                        opacity: (zik.autoNC && (zik.ncEnabled == "true")) ? 1.0 : 0.0
                        text: "AUTO"
                        anchors.centerIn: parent

                        Behavior on opacity { NumberAnimation { duration: 500 } }

                        rotation: -90
                    }
                }

                Switch{
                    z:1
                    id: ncSwitch

                    anchors.horizontalCenter: parent.horizontalCenter

                    checked: zik.ncEnabled == "true" ? true : false

                    automaticCheck: false

                    property bool thirdStateEnabled: zik.capabilities & Zik.AutoNc

                    onClicked: {
                        if(thirdStateEnabled){
                            if(zik.ncEnabled == "false"){
                                zik.autoNC = false;
                                zik.ncEnabled = "true";
                            }else{
                                if(zik.autoNC == false){
                                    zik.autoNC = true;
                                }else{
                                    zik.autoNC = false;
                                    zik.ncEnabled = "false";
                                }
                            }
                        }else{
                            zik.autoNC = false;

                            if(zik.ncEnabled == "false"){
                                zik.ncEnabled = "true";
                            }else{
                                zik.ncEnabled = "false";
                            }
                        }
                    }

                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: Theme.highlightColor

                        visible: zik.autoNC && (zik.ncEnabled == "true")
                    }
                }
            }

            Column {
                id: caRow

                width: parent.elementWidth
                height: width + caSwitch.height

                ConcertHall {
                    id: concertHall

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: parent.width
                    height: width

                    borderColor: Theme.primaryColor
                    activeBorderColor: Theme.primaryColor
                    fillColor: "transparent"
                    activeFillColor: Theme.secondaryHighlightColor
                    handlesColor: Theme.highlightColor
                    handleDiameter: width * 1/10

                    circleOpacity: 0.4
                    handleOpacity: 0.4

                    surround: zik.surroundMode  // leave it declared before angle ! Important ! Order matters.
                    room: zik.roomSize
                    angle: zik.spatializerAngle

                    handleImage: Qt.resolvedUrl("image://theme/icon-m-speaker")

                    onAngleChanged: {
                        zik.spatializerAngle = angle;
                    }

                    onRoomChanged: {
                        zik.roomSize = room;
                    }
                }

                Switch{
                    z:1
                    id: caSwitch

                    checked: zik.concertHall == "true" ? true : false
                    onCheckedChanged: {
                        if(checked){
                            zik.concertHall = "true"
                        }else{
                            zik.concertHall = "false"
                        }
                    }

                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

            Column {
                id: eqRow

                width: parent.elementWidth
                height: width + eqSwitch.height

                ThumbEqualizer{
                    width: parent.width
                    height: width

                    anchors.horizontalCenter: parent.horizontalCenter

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


                Switch{
                    z:1
                    id: eqSwitch

                    checked: zik.equalizer == "true" ? true : false
                    onCheckedChanged: {
                        if(checked){
                            zik.equalizer = "true"
                        }else{
                            zik.equalizer = "false"
                        }
                    }

                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        } // second line row


    } // silica flickable

}


