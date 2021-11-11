import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0
import "qrc:/elements"
import "../"

Page {
    id: page

    Connections {
        target: zik
        onDisconnected: {
            pageStack.push(Qt.resolvedUrl("../LoadingPage.qml"));
        }
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors {
            fill: parent
            bottomMargin: metadataPanel.margin
        }

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
                eq.checked = zik.equalizer == "true" ? true : false;
            }
            onNcEnabledChanged: {
                nc.checked = zik.ncEnabled == "true" ? true : false;
            }
            onConcertHallChanged: {
                ca.checked =  zik.concertHall == "true" ? true : false;
            }
        }


        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height



        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: zik.demoMode ? "Zik 3 Manager (demo mode)" : zik.model == "ZIK3" ? "Zik 3 Manager" : zik.model == "ZIK2" ? "Zik 2.0 Manager" : zik.model == "ZIKSPORT" ? "Zik Sport Manager" : "Zik 1 Manager (legacy)"
            }

            BatteryStatus{
                anchors.horizontalCenter: parent.horizontalCenter

                width: Screen.sizeCategory > Screen.Medium ? Math.min(parent.width * 2/3, page.height * 1/3) : parent.width * 2/3
                height: width

                percent: zik.battery
                timeleft: -1

                fontFamily: Theme.fontFamily
                textColor: Theme.primaryColor
            }

            BackgroundItem{
                z:0

                id: ncBg

                highlightedColor: Theme.secondaryHighlightColor

                    Switch{
                        z:1
                        id: nc
                        checked:  (zik.capabilities & Zik.StreetMode) ? (zik.ncEnabled == "true" ? true : false) : (zik.anc == "true" ? true : false)
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

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        id: ncText
                        text: qsTr("Noise Control")
                        color: ncBg.down ? Theme.highlightColor : Theme.primaryColor

                        anchors.left: nc.right
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Image {
                        source: "image://theme/icon-l-right"

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        anchors.rightMargin: 20

                        visible: zik.capabilities & Zik.StreetMode
                    }


                onClicked: {
                    pageStack.push(Qt.resolvedUrl("NoiseControlPage.qml"));
                }
            }


            BackgroundItem{
                z:0

                id: caBg

                highlightedColor: Theme.secondaryHighlightColor

                    Switch{
                        z:1
                        id: ca

                        checked: zik.concertHall == "true" ? true : false
                        onCheckedChanged: {
                            if(checked){
                                zik.concertHall = "true"
                            }else{
                                zik.concertHall = "false"
                            }
                        }

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        id: caText
                        text: qsTr("Concert Hall")
                        color: caBg.down ? Theme.highlightColor : Theme.primaryColor

                        anchors.left: ca.right
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Image {
                        source: "image://theme/icon-l-right"

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        anchors.rightMargin: 20
                    }


                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ConcertHallPage.qml"));
                }
            }


            BackgroundItem{
                z:0

                id: eqBg

                visible: zik.capabilities & Zik.ThumbEqualizer

                highlightedColor: Theme.secondaryHighlightColor

                    Switch{
                        z:1
                        id: eq

                        checked: zik.equalizer == "true" ? true : false
                        onCheckedChanged: {
                            if(checked){
                                zik.equalizer = "true"
                            }else{
                                zik.equalizer = "false"
                            }
                        }

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        id: eqText
                        text: qsTr("Equalizer")
                        color: eqBg.down ? Theme.highlightColor : Theme.primaryColor

                        anchors.left: eq.right
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Image {
                        source: "image://theme/icon-l-right"

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        anchors.rightMargin: 20
                    }


                onClicked: {
                    pageStack.push(Qt.resolvedUrl("EqualizerPage.qml"));
                }
            }

        }
    }

    DockedPanel{
        id: metadataPanel

        width: parent.width
        height: Theme.itemSizeMedium + Theme.paddingLarge

        dock: Dock.Bottom

        state: "hidden"

        property bool metadataEmpty: (zik.artist == "") && (zik.album == "") && (zik.song == "")
        property bool musicPlaying: !zik.playing ? false : zik.source != "line-in" ? true : false
        property bool shouldBeDisplayed: (!musicPlaying) || metadataEmpty ? false : true

        onShouldBeDisplayedChanged: {
            if(shouldBeDisplayed == true){
                t_meta_hide.stop();
                state = "visible";
            }else{
                t_meta_hide.start();
            }
        }

        Behavior on opacity { NumberAnimation{ duration: 1000 } }

        onStateChanged: {
            console.log(state);
        }

        states: [
            State {
                name: "visible"
                PropertyChanges { target: metadataPanel; open: true}
            },
            State {
                name: "hidden"
                PropertyChanges { target: metadataPanel; open: false}
            }
        ]

        Column{
            anchors.fill: parent
            anchors.margins: 20
            spacing: (height - (Theme.fontSizeSmall*2)) / 2
            Text {
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall

                text: zik.artist + " - " + zik.album
            }

            Text {
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall

                text: zik.song
            }
        }

        Timer {
            id: t_meta_hide
            interval: 3000
            onTriggered: {
                metadataPanel.state = "hidden";
            }
        }


    }
}


