import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0

Page {
    id: settingsPage

    allowedOrientations: Screen.sizeCategory > Screen.Medium ? Orientation.All : Orientation.Portrait

    Connections {
             target: zik
             onFriendlyNameChanged: friendlyName.text = ""
    }


   SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView

        contentHeight: col.height

        PullDownMenu {
            MenuItem {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column{
            id: col

            width: settingsPage.width
            spacing: Theme.paddingSmall

            PageHeader {
                title: "Settings"
            }

            TextField{
                id: friendlyName
                label : "Friendly Name: "
                width: parent.width
                placeholderText: "Friendly name : " + zik.friendlyName

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    focus = false
                    if(friendlyName.text !== ""){
                        zik.setFriendlyName(friendlyName.text);
                    }
                }
            }

            ComboBox {
                id: cbAPO

                label: "Auto Poweroff (minutes)"

                Connections {
                    target: zik
                    onAutoPowerOffChanged: {
                        cbAPO.updateSelectedItem();
                    }
                }

                Component.onCompleted: {
                    updateSelectedItem();
                }

                function updateSelectedItem(){
                    var i;
                    for(i=0; i < 6; i++){
                        if(zik.autoPowerOff == parseInt(cbItems.children[i].text)){

                            print("well shit");
                            currentIndex = i;
                            break;
                        }
                    }
                }

                menu: ContextMenu {
                    id: cbItems
                    MenuItem { text: "Disabled" }
                    MenuItem { text: "5" }
                    MenuItem { text: "10" }
                    MenuItem { text: "15" }
                    MenuItem { text: "30" }
                    MenuItem { text: "60" }
                }

                onCurrentIndexChanged: {
                    zik.autoPowerOff = parseInt(value);
                }
            }
/*
            ComboBox {
                menu: ContextMenu {
                    MenuItem { text: "anc transport" }
                    MenuItem { text: "anc quiet" }
                    MenuItem { text: "off" }
                    MenuItem { text: "aoc quiet" }
                    MenuItem { text: "aoc transport" }
                }

                onValueChanged: {
                    switch(value){
                        case "anc transport": zik.setPhoneModeNc("anc", 2); break;
                        case "anc quiet": zik.setPhoneModeNc("anc", 1); break;
                        case "off": zik.setPhoneModeNc("off", 1); break;
                        case "aoc quiet": zik.setPhoneModeNc("aoc", 1); break;
                        case "aoc transport": zik.setPhoneModeNc("aoc", 2); break;
                    }
                }
            }
*/
            Switch {
                z: 1
                id: headDetect

                checked: zik.headDetectionEnabled
                onCheckedChanged: {
                    zik.headDetectionEnabled = checked;
                }

                Text {
                    id: headDetectText
                    text: qsTr("Head detection")
                    color: Theme.primaryColor

                    anchors.left: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Switch {
                z: 1
                id: autoConnect

                checked: zik.autoConnectionEnabled
                onCheckedChanged: {
                    zik.autoConnectionEnabled = checked;
                }

                Text {
                    id: autoConnectText
                    text: qsTr("Auto connection")
                    color: Theme.primaryColor

                    anchors.left: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Switch{
                z: 1
                id: tts

                checked: zik.ttsEnabled
                onCheckedChanged: {
                    zik.ttsEnabled = checked
                }

                visible: (zik.capabilities & Zik.TTS) && (zik.ttsLang != "none")
                enabled: zik.ttsLang != "none"

                Text {
                    id: ttsText
                    text: qsTr("Text to Speech")
                    color: Theme.primaryColor

                    anchors.left: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Switch{
                z: 1
                id: flight

                visible: zik.capabilities & Zik.FlightMode

                checked: zik.flightModeEnabled
                onCheckedChanged: {
                    zik.flightModeEnabled = checked
                }

                Text {
                    id: flightText
                    text: qsTr("Flight mode")
                    color: Theme.primaryColor

                    anchors.left: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Slider{
                id: delaySlider

                visible: zik.capabilities & Zik.BluetoothDelay

                anchors.left: parent.left
                anchors.right: parent.right

                label: qsTr("Bluetooth streaming delay")
                minimumValue: 40
                maximumValue: 280
                stepSize: 10

                value: zik.a2dpDelay
                valueText: sliderValue + "ms"

                onReleased: {
                    zik.a2dpDelay = sliderValue
                }
            }

        }
    }
}





