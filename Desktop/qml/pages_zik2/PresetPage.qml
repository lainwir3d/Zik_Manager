import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

import "../elements/desktop"

Item {
    id: page

    CustomButton {
         id: backButton

         anchors.margins: 5
         anchors.top: parent.top
         anchors.left: parent.left

         text: "Back"
         textColor: "white"

         onClicked: {
             page.Stack.view.pop();
         }
     }


    CustomButton {
         id: fwdButton

         anchors.margins: 5
         anchors.top: parent.top
         anchors.right: parent.right

         text: "New Preset"
         textColor: "white"

         onClicked: {
             page.Stack.view.push({item: Qt.resolvedUrl("ConcertHallPage.qml"), properties: {producerMode: 1}})
         }
    }

    LabeledSwitch {
        id:smartAudioTune

        text: "Smart Audio Tune"
        textColor: "white"

        anchors.top: backButton.bottom
        anchors.left: parent.left
        anchors.margins: 5

        height: 40

        switchCheckedColor: "orange"
        switchUncheckedColor: "white"

        checked: zik.smartAudioTune == "true" ? true : false
        onCheckedChanged: {
            zik.smartAudioTune = checked ? "true" : "false"
        }
    }

    ScrollView{
        anchors.top: smartAudioTune.bottom
        anchors.left: page.left
        anchors.right: page.right
        anchors.leftMargin: 10
        anchors.topMargin: 20

        height: page.height - backButton.height - smartAudioTune.height

        style: ScrollViewStyle {
            transientScrollBars: true
            handle: Item {
                implicitWidth: 14
                implicitHeight: 26
                Rectangle {
                    color: "#424246"
                    anchors.fill: parent
                    anchors.topMargin: 6
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.bottomMargin: 6
                }
            }
            scrollBarBackground: Item {
                implicitWidth: 14
                implicitHeight: 26
            }
        }

        ListView {
            id: presetView
            anchors.fill: parent

            spacing: 5

            model: presetModel
            delegate: RowLayout{
                property var view: parent

                LabeledSwitch {
                    property bool status : p_enabled

                    text: id +  ": " + name
                    textColor: "white"

                    switchCheckedColor: "orange"
                    switchUncheckedColor: "white"

                    height: 40

                    Component.onCompleted: checked = p_enabled
                    onCheckedChanged: {
                        if(checked != p_enabled){
                            zik.enablePreset(id, checked);
                        }
                    }

                    onStatusChanged: {
                        console.log("id " + id + "changed");
                        checked = p_enabled
                    }
                }

                Button{
                    text: "Delete"
                    onClicked: {
                        zik.deletePreset(id);
                    }

                }

            }

        }
    }

}
