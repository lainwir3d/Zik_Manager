import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0

import "../elements/desktop"

Item {
    id: firstPage

    CustomButton {
         id: backButton

         anchors.margins: 5
         anchors.top: parent.top
         anchors.left: parent.left

         text: "Back"
         textColor: "white"

         onClicked: {
             firstPage.Stack.view.pop();
         }
    }

    CustomButton {
         id: fwdButton

         anchors.margins: 5
         anchors.top: parent.top
         anchors.right: parent.right

         text: "Save"
         textColor: "white"

         onClicked: {
             firstPage.Stack.view.pop({item: firstPage.Stack.view.get(1, true)});
             zik.savePreset(nameField.text, albumField.text, albumEnabled.checked, artistField.text, artistEnabled.checked, genreField.text, genreEnabled.checked, songField.text, songEnabled.checked, false);
         }
    }

    Image{
        id: cover
        anchors.top: backButton.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10

        width: parent.width * 0.5
        height: width
        fillMode: Image.PreserveAspectFit
        clip: true

        visible: status == Image.Ready ? true : false

        source: zik.artUrl
        smooth: true
        mipmap: true
        opacity: 0.7
    }

    ColumnLayout{

        anchors.top: cover.visible ? cover.bottom : backButton.bottom
        anchors.left: parent.left
        anchors.margins: 10

        spacing: 10

        RowLayout{

            CustomTextField {
                id: nameField
                placeholder: "Enter preset name"
                textColor: "white"

                label: "Preset name"

                anchors.verticalCenter: parent.verticalCenter
            }
        }


        RowLayout{

            LabeledSwitch {
                id: artistEnabled

                height: 40

                switchCheckedColor: "orange"
                switchUncheckedColor: "white"

                text: ""
                textColor: "white"

                checked: false
            }

            CustomTextField {
                id: artistField
                enabled: artistEnabled.checked
                placeholder: "Enter artist name"
                label: "Artist"
                textColor: "white"
                text: zik.artist

                width: 30
            }
        }

        RowLayout{

            LabeledSwitch {
                id: albumEnabled

                height: 40

                switchCheckedColor: "orange"
                switchUncheckedColor: "white"

                text: ""
                textColor: "white"

                checked: false
            }

            CustomTextField {
                id: albumField
                enabled: albumEnabled.checked
                placeholder: "Enter album name"
                label: "Album"
                textColor: "white"
                text: zik.album
            }
        }

        RowLayout{

            LabeledSwitch {
                id: genreEnabled

                height: 40

                switchCheckedColor: "orange"
                switchUncheckedColor: "white"

                text: ""
                textColor: "white"

                checked: false
            }

            CustomTextField {
                id: genreField
                enabled: genreEnabled.checked
                placeholder: "Enter genre name"
                label: "Genre"
                textColor: "white"
                text: zik.genre
            }
        }

        RowLayout{

            LabeledSwitch {
                id: songEnabled

                height: 40

                switchCheckedColor: "orange"
                switchUncheckedColor: "white"

                text: ""
                textColor: "white"

                checked: false
            }

            CustomTextField {
                id: songField
                enabled: songEnabled.checked
                placeholder: "Enter song name"
                label: "Song"
                textColor: "white"
                text: zik.song
            }
        }
    }

}
