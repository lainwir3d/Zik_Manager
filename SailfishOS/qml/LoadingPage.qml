import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    backNavigation: false
    showNavigationIndicator: false

    allowedOrientations: Screen.sizeCategory > Screen.Medium ? Orientation.All : Orientation.Portrait

    SilicaFlickable {
        anchors {
            fill: parent
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView

        PullDownMenu {
            MenuItem {
                text: qsTr("Demo mode")
                onClicked: zik.demoMode = true
            }
        }

        Loader {
            id: loader
            anchors.centerIn: parent
        }

        PageHeader {
            title: zik.bluetoothOn ? "Connecting" : "Bluetooth off"
        }


        Component{
            id: bluetoothoff

            Text {
                color: Theme.primaryColor

                text: "Please turn bluetooth on."
            }
        }

        Component {
            id: connecting

            Item {
                anchors.centerIn: parent
                BusyIndicator{
                    id: busy
                    anchors.centerIn: parent

                    running: Qt.application.active
                    size: BusyIndicatorSize.Large
                }


                Text {
                    id: loadingText

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: busy.bottom
                    anchors.topMargin: 20

                    color: Theme.primaryColor
                    state: "1"

                    property string baseText: qsTr("Looking for a Parrot Zik device%1")

                    states: [
                        State {
                            name: "1"
                            PropertyChanges {
                                target: loadingText
                                text: loadingText.baseText.arg(".  ")
                            }
                        },
                        State {
                            name: "2"
                            PropertyChanges {
                                target: loadingText
                                text: loadingText.baseText.arg(".. ")
                            }
                        },
                        State {
                            name: "3"
                            PropertyChanges {
                                target: loadingText
                                text: loadingText.baseText.arg("...")
                            }
                        }
                    ]

                    Timer {
                        id: t1
                        interval: 300
                        running: Qt.application.active
                        repeat: true
                        onTriggered: {
                            if(loadingText.state == "1") loadingText.state = "2";
                            else if(loadingText.state == "2") loadingText.state = "3";
                            else if(loadingText.state == "3") loadingText.state = "1";
                        }
                    }
                }
            }

        }
    }

    Component.onCompleted: {
        if(zik.bluetoothOn){
            loader.sourceComponent = connecting;
        }else{
            loader.sourceComponent = bluetoothoff;
        }
    }

    Connections {
        target: zik
        onConnected: {
            if(pageStack.depth == 1){
                if(orientation & Orientation.LandscapeMask){
                    if(zik.model == "ZIK1"){
                        pageStack.replaceAbove(0, Qt.resolvedUrl("pages_zik2/FirstPage.qml"));
                    }else{
                        pageStack.replaceAbove(0, Qt.resolvedUrl("pages_zik2/WideLandscape.qml"));
                    }
                }else{
                    pageStack.replace(Qt.resolvedUrl("pages_zik2/FirstPage.qml"));
                }
            }else{
                pageStack.pop();
            }
        }
        onDisconnected: connected = false

        onBluetoothOnChanged: {
            if(zik.bluetoothOn){
                loader.sourceComponent = connecting;
            }else{
                loader.sourceComponent = bluetoothoff;
            }
        }
    }

}
