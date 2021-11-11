import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import ZikManager 1.0

import "../elements/desktop"

Item {
    id: firstPage

    property int baseMargin: 10

    Connections {
        target: zik
        onTtsEnabledChanged: {
            tts.checked = zik.ttsEnabled;
        }
        onFlightModeEnabledChanged: {
            flight.checked = zik.flightModeEnabled;
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
    Stack.onStatusChanged: {
            if(Stack.status == Stack.Active){
                firstPage.forceActiveFocus();
            }
    }

    CustomButton {
        id: aboutButton

        anchors.margins: firstPage.baseMargin
        anchors.top: parent.top
        anchors.right: parent.right

        text: "About"
        textColor: "white"

        onClicked: {
            firstPage.Stack.view.push({item: Qt.resolvedUrl("AboutPage.qml"), properties: {baseMargin: firstPage.baseMargin}})
        }
    }

    Flickable {
        anchors.top: backButton.bottom
        anchors.margins: firstPage.baseMargin
        anchors.topMargin: firstPage.baseMargin * 2

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: width
        contentHeight: col.height

        Column{
            id: col

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: firstPage.baseMargin

            CustomTextField {
                id: nameField

                placeholder: zik.friendlyName
                textColor: "white"

                label: "Friendly name"

                field.onAccepted: {
                    zik.friendlyName = text;
                    text = "";
                }
            }

            Item {
                height: cbAPO.implicitHeight
                width: parent.width

                Label {
                    id: labAPO
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    text: "Auto poweroff: "
                    color: "white"
                }

                ComboBox {
                    id: cbAPO

                    anchors.left: labAPO.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: firstPage.baseMargin

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
                        for(i=0; i < cbItems.count; i++){
                            if(zik.autoPowerOff == cbItems.get(i).time){
                                currentIndex = i;
                                break;
                            }
                        }
                    }

                    model: ListModel {
                        id: cbItems
                        ListElement { text: ""; time: 0 }
                        ListElement { text: "0 minutes"; time: 0 }
                        ListElement { text: "5 minutes"; time: 5 }
                        ListElement { text: "10 minutes"; time: 10 }
                        ListElement { text: "15 minutes"; time: 15 }
                        ListElement { text: "30 minutes"; time: 30 }
                        ListElement { text: "45 minutes"; time: 45 }
                        ListElement { text: "60 minutes"; time: 60 }
                    }

                    onCurrentIndexChanged: {
                        if(cbItems.get(currentIndex).text != ""){
                            zik.autoPowerOff = cbItems.get(currentIndex).time;
                        }
                    }
                }
            }


            LabeledSwitch {
                id: headDetect

                height: width / 10
                width: parent.width

                baseMargin: 10

                text: "Head detection"
                textColor: "white"

                checked: zik.headDetectionEnabled
                onCheckedChanged: {
                    zik.headDetectionEnabled = checked;
                }
            }

            /*
        LabeledSwitch {
            id: touchPanel

            height: width / 10
            width: parent.width

            baseMargin: 10

            text: "Touch panel"
            textColor: "white"

            checked: zik.touchPanelEnabled
            onCheckedChanged: {
                zik.touchPanelEnabled = checked;
            }
        }
        */

            LabeledSwitch {
                id: autoConnect

                height: width / 10
                width: parent.width

                baseMargin: 10

                text: "Auto connection"
                textColor: "white"

                checked: zik.autoConnectionEnabled
                onCheckedChanged: {
                    zik.autoConnectionEnabled = checked;
                }
            }

            LabeledSwitch {
                id: tts

                height: width / 10
                width: parent.width

                baseMargin: 10

                text: "Text to Speech"
                textColor: "white"

                checked: zik.ttsEnabled
                onCheckedChanged: {
                    zik.ttsEnabled = checked;
                }

                visible: (zik.capabilities & Zik.TTS) && (zik.ttsLang != "none")
                enabled: zik.ttsLang != "none"
            }

            LabeledSwitch {
                id: flight

                visible: zik.capabilities & Zik.FlightMode

                height: width / 10
                width: parent.width

                baseMargin: 10

                text: "Flight mode"
                textColor: "white"

                checked: zik.flightModeEnabled
                onCheckedChanged: {
                    zik.flightModeEnabled = checked
                }
            }

            Item{
                height: delayTxt.paintedHeight + delaySlider.height

                anchors.left: parent.left
                anchors.right: parent.right

                visible: zik.capabilities & Zik.BluetoothDelay

                Label{
                    id: delayTxt
                    text: "Bluetooth streaming delay : " + zik.a2dpDelay + (delaySlider.pressed ? (" → " + delaySlider.value) : "")

                    color: "white"

                    anchors.bottom: delaySlider.top
                    anchors.bottomMargin: 0
                }

                Slider{
                    id: delaySlider

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    minimumValue: 40
                    maximumValue: 280
                    stepSize: 10
                    updateValueWhileDragging: true

                    value: zik.a2dpDelay

                    onPressedChanged: {
                        if(!pressed){
                            zik.a2dpDelay = value
                        }
                    }
                }
            }
        }
    }
}
