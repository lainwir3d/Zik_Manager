import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0

Page {
    id: aboutPage

    allowedOrientations: Screen.sizeCategory > Screen.Medium ? Orientation.All : Orientation.Portrait

        Column{
            width: aboutPage.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: "About"
            }

            TextArea {
                label : "Software information"
                width: parent.width
                readOnly: true

                Component.onCompleted: {
                _editor.textFormat = TextEdit.RichText;
                }

                text: "An unofficial native manager for the Parrot Zik headphones range (1 / 2.0 / 3). For more info on the Parrot Zik headphones please see the official products page : http://www.parrot.com/usa/products"
            }

            Separator {
                width: parent.width
                height: 3
                color: Theme.primaryColor
            }

            TextField{
                label : "Model"
                width: parent.width
                text: zik.model == "ZIK1" ? "Parrot Zik 1" : zik.model == "ZIK2" ? "Parrot Zik 2.0" : zik.model == "ZIK3" ? "Parrot Zik 3" : zik.model == "ZIKSPORT" ? "Parrot Zik Sport" : "Unknown"
                readOnly: true
            }

            TextField{
                label : "Headphone firmware version"
                width: parent.width
                text: zik.version
                readOnly: true
            }

            TextField{
                label : "Text to speech language"
                width: parent.width
                text: zik.ttsLangHumanReadable != "none" ? zik.ttsLangHumanReadable : "No language pack installed"
                readOnly: true

                visible: zik.capabilities & Zik.TTS
            }
/*
            TextField{
                label : "PIC version"
                width: parent.width
                text: zik.picVersion
                readOnly: true
            }
*/

            Separator {
                width: parent.width
                height: 3
                color: Theme.primaryColor
            }

            TextField{
                label : "Developer"
                width: parent.width
                text: "lainwir3d <lainwir3d@gmail.com>"
                readOnly: true
            }
        }

}





