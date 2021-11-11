import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import ZikManager 1.0

import "../elements/desktop"

Item {
    id: firstPage

    property int baseMargin: 10

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

            CustomTextArea{
                width: parent.width
                label: qsTr("Software information")
                text: "An unofficial native manager for the Parrot Zik headphones range (1 / 2.0 / 3). For more info on the Parrot Zik headphones please see the official products page : http://www.parrot.com/usa/products"
                readOnly: true
            }

            CustomSeparator {
                height: 1
                width: parent.width
            }

            CustomTextField{
                label : "Model"
                width: parent.width
                text: zik.model == "ZIK1" ? "Parrot Zik 1" : zik.model == "ZIK2" ? "Parrot Zik 2.0" : zik.model == "ZIK3" ? "Parrot Zik 3" : "Unknown"
                readOnly: true
            }

            CustomTextField{
                label : "Headphone firmware version"
                width: parent.width
                text: zik.version
                readOnly: true
            }

            CustomTextField{
                label : "Text to speech language"
                width: parent.width
                text: zik.ttsLangHumanReadable != "none" ? zik.ttsLangHumanReadable : "No language pack installed"
                readOnly: true

                visible: zik.capabilities & Zik.TTS
            }

            CustomSeparator {
                width: parent.width
                height: 1
            }

            CustomTextField{
                label : "Developer"
                width: parent.width
                text: "lainwir3d <lainwir3d@gmail.com>"
                readOnly: true
            }
        }

    }
}
