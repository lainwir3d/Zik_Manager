import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import ZikManager 1.0

import "../elements/"
import "../elements/desktop"

Item {
    id: firstPage

    //property string transitionName: "fadeTransition"

    property int baseMargin: width / 30

    property bool isPortrait: Screen.primaryOrientation === Qt.PortraitOrientation || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    /*
    onIsPortraitChanged: {
       print("orientation!");
    }
    */

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        zik.setAccountUsername("QZik");

        if(zik.capabilities & Zik.StreetMode){
            // feature support check example !
        }

    }

    Connections {
        target: zik
        onEqualizerChanged: {
            if(zik.capabilities & Zik.ThumbEqualizer){
                eq.checked = zik.equalizer == "true" ? true : false;
            }else{
                zik1eq.checked = (zik.equalizer == "true") ? true : false;
            }
        }
        onNcEnabledChanged: {
            if(zik.capabilities & Zik.StreetMode){
                nc.checked = zik.ncEnabled == "true" ? true : false;
            }
        }
        onAncChanged: {
            if(!(zik.capabilities & Zik.StreetMode)){
                nc.checked = zik.anc == "true" ? true : false;
            }
        }
        onConcertHallChanged: {
            ca.checked =  zik.concertHall == "true" ? true : false;
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
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        width: height
        height: (parent.height * 5/12) < parent.width ? (parent.height * 5/12) : parent.width

        percent: zik.battery
        timeleft: -1

        fontFamily: nc.font.family
    }

    Image{
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        width: parent.width / 10
        height: width

        source: Qt.resolvedUrl("icon-camera-settings.png")

        MouseArea {
            anchors.fill: parent

            onClicked: {
                firstPage.Stack.view.push({item: Qt.resolvedUrl("SettingsPage.qml"), properties: {baseMargin: firstPage.baseMargin}})
            }
        }
    }

    ColumnLayout{
        id: layout

        anchors.top: batteryWidget.bottom
        anchors.bottom: metapanel.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 0
        anchors.bottomMargin: 20

        property int nbElements: 3

        LabeledSwitch {
            id: nc

            //Layout.minimumHeight: 40
            Layout.preferredHeight: (parent.height / parent.nbElements) - firstPage.baseMargin * 4
            Layout.maximumHeight: (parent.height / 3) - firstPage.baseMargin * 4
            Layout.fillHeight: true
            Layout.fillWidth: true

            baseMargin: 10
            iconMargin: 3
            iconLeftMargin: 10

            text: "Noise Control"
            textColor: "white"

            backgroundPressedColor: "black"
            backgroundPressedOpacity: 0.5

            iconSource: Qt.resolvedUrl("arrow.svg")
            iconColor: "white"
            iconOpacity: zik.capabilities & Zik.StreetMode ? 0.4 : 0.0

            checked: (zik.capabilities & Zik.StreetMode) ? (zik.ncEnabled == "true" ? true : false) : (zik.anc == "true" ? true : false)
            onCheckedChanged: {
                if(zik.capabilities & Zik.StreetMode){
                    if(checked){
                        zik.ncEnabled = "true";
                    }else{
                        zik.ncEnabled = "false";
                    }
                }else{
                    if(checked){
                        zik.anc = "true";
                    }else{
                        zik.anc = "false";
                    }
                }
            }

            onLabelClicked: {
                if(zik.capabilities & Zik.StreetMode){
                    firstPage.Stack.view.push({item: Qt.resolvedUrl("NoiseControlPage.qml"), properties: {producerMode: 0, baseMargin: firstPage.baseMargin}});
                }
            }
        }

        LabeledSwitch {
            id:ca

            //Layout.minimumHeight: 40
            Layout.preferredHeight: (parent.height / parent.nbElements) - firstPage.baseMargin * 4
            Layout.maximumHeight: (parent.height / 3) - firstPage.baseMargin * 4
            Layout.fillHeight: true
            Layout.fillWidth: true

            baseMargin: 10
            iconMargin: 3
            iconLeftMargin: 10

            text: "Concert Hall"
            textColor: "white"

            switchCheckedColor: zik.currentId == 0 ? "orange" : "cornflowerblue"
            switchUncheckedColor: zik.currentId == 0 ? "white" : "red"

            checked: zik.concertHall == "true" ? true : false

            backgroundPressedColor: "black"
            backgroundPressedOpacity: 0.5

            iconSource: Qt.resolvedUrl("arrow.svg")
            iconColor: "white"
            iconOpacity: 0.4

            onCheckedChanged: {
                if(checked){
                    zik.concertHall = "true"
                }else{
                    zik.concertHall = "false"
                }
            }

            onLabelClicked: {
                firstPage.Stack.view.push({item: Qt.resolvedUrl("ConcertHallPage.qml"), properties: {producerMode: 0, baseMargin: firstPage.baseMargin}})
            }

        }

        LabeledSwitch {
            id: eq

            visible: zik.capabilities & Zik.ThumbEqualizer

            //Layout.minimumHeight: 40
            Layout.preferredHeight: (parent.height / parent.nbElements)  - firstPage.baseMargin * 4
            Layout.maximumHeight: (parent.height / 3) - firstPage.baseMargin * 4
            Layout.fillHeight: true
            Layout.fillWidth: true

            baseMargin: 10
            iconMargin: 3
            iconLeftMargin: 10

            text: "Equalizer"
            textColor: "white"

            switchCheckedColor: zik.currentId == 0 ? "orange" : "cornflowerblue"
            switchUncheckedColor: zik.currentId == 0 ? "white" : "red"

            backgroundPressedColor: "black"
            backgroundPressedOpacity: 0.5

            iconSource: Qt.resolvedUrl("arrow.svg")
            iconColor: "white"
            iconOpacity: 0.4

            checked: zik.equalizer == "true" ? true : false
            onCheckedChanged: {
                if(checked){
                    zik.equalizer = "true"
                }else{
                    zik.equalizer = "false"
                }
            }

            onLabelClicked: {
                firstPage.Stack.view.push({item: Qt.resolvedUrl("EqualizerPage.qml"), properties: {baseMargin: firstPage.baseMargin}});
            }
        }

        LabeledSwitch {
            id: zik1eq

            visible: !(zik.capabilities & Zik.ThumbEqualizer)

            Layout.minimumHeight: 40
            Layout.preferredHeight: (parent.height / parent.nbElements)  - parent.spacing*8
            Layout.maximumHeight: (parent.height / parent.nbElements)
            Layout.fillHeight: false
            Layout.fillWidth: true

            baseMargin: 10
            iconMargin: 3
            iconLeftMargin: 10

            width: parent.width

            text: "Equalizer: " + zik1PresetModel.get(zik.oldPresetId).zik1PresetName
            textColor: "white"

            switchCheckedColor: "orange"
            switchUncheckedColor: "white"

            backgroundPressedColor: "black"
            backgroundPressedOpacity: 0.5

            iconSource: Qt.resolvedUrl("arrow.svg")
            iconColor: "white"
            iconOpacity: 0.0

            checked: {
                return (zik.equalizer == "true") ? true : false
            }
            onCheckedChanged: {
                if(checked){
                    zik.equalizer = "true"
                }else{
                    zik.equalizer = "false"
                }
            }

            onLabelClicked: {
                zik.oldPresetId = (zik.oldPresetId >= 5) ? 0 : zik.oldPresetId + 1;

                if(zik.equalizer == "false"){
                    zik.equalizer = "true";
                }
            }

            ListModel {
                id: zik1PresetModel

                ListElement {
                    zik1PresetId: 0
                    zik1PresetName: "Vocal"
                }

                ListElement {
                    zik1PresetId: 1
                    zik1PresetName: "Pop"
                }

                ListElement {
                    zik1PresetId: 2
                    zik1PresetName: "Club"
                }

                ListElement {
                    zik1PresetId: 3
                    zik1PresetName: "Punchy"
                }

                ListElement {
                    zik1PresetId: 4
                    zik1PresetName: "Deep"
                }

                ListElement {
                    zik1PresetId: 5
                    zik1PresetName: "Cristal"
                }

                ListElement {
                    zik1PresetId: 6
                    zik1PresetName: "User"
                }
            }
        }

    }


    Item{
        id: metapanel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        width:parent.width
        //height: parent.height * 1/10
        height: musicPlaying && !metadataEmpty ? parent.height * 1/10 : 0.0

        Behavior on height { NumberAnimation { duration: 500; easing.type: Easing.Linear } }

        property bool metadataEmpty: (zik.artist == "") && (zik.album == "") && (zik.song == "")
        property bool musicPlaying: !zik.playing ? false : zik.source != "line-in" ? true : false

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

        /*
        Image{
            anchors.right: parent.right
            anchors.rightMargin: 10

            height: parent.height * 0.9
            width: height

            source: zik.artUrl

        }
        */

        Column{
            z:7
            anchors.fill: parent
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
