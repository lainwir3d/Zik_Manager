import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: savePresetPage

    Column{
        id: column
        width: savePresetPage.width
        spacing: Theme.paddingSmall

        DialogHeader {
            title: "Save preset"
        }

        Image{
            id: cover
            anchors.margins: 10

            width: parent.width * 0.5
            height: width
            fillMode: Image.PreserveAspectFit
            clip: true

            visible: status == Image.Ready ? true : false

            source: zik.artUrl
            smooth: true
            opacity: 0.7
        }


        TextField {
            id: name
            label : "Name"
            width: parent.width
            placeholderText: "Name : "
        }

        Row{
            width: parent.width
            spacing: Theme.paddingLarge * -1

            Switch{
                z:1
                id: artistEn

                anchors.verticalCenter: parent.verticalCenter

                checked: false

            }

            TextField {
                id: artist
                width: column.width - artistEn.width

                anchors.verticalCenter: parent.verticalCenter

                label : "Artist"
                placeholderText: "Artist : "
                text: zik.artist
            }
        }

        Row{
            width: parent.width
            spacing: Theme.paddingLarge * -1

            Switch{
                z:1
                id: albumEn

                anchors.verticalCenter: parent.verticalCenter

                checked: false

            }

            TextField {
                id: album
                width: column.width - albumEn.width

                anchors.verticalCenter: parent.verticalCenter

                label : "Album"
                placeholderText: "Album : "
                text: zik.album
            }
        }

        Row{
            width: parent.width
            spacing: Theme.paddingLarge * -1

            Switch{
                z:1
                id: titleEn

                anchors.verticalCenter: parent.verticalCenter

                checked: false

            }

            TextField {
                id: title
                width: column.width - titleEn.width

                anchors.verticalCenter: parent.verticalCenter

                label : "Title"
                placeholderText: "Title : "
                text: zik.song
            }
        }

        Row{
            width: parent.width
            spacing: Theme.paddingLarge * -1

            Switch{
                z:1
                id: genreEn

                anchors.verticalCenter: parent.verticalCenter

                checked: false

            }

            TextField {
                id: genre
                width: column.width - genreEn.width

                anchors.verticalCenter: parent.verticalCenter

                label : "Genre"
                placeholderText: "Genre : "
                text: zik.genre
            }
        }
    }

    onDone: {
        if (result == DialogResult.Accepted) {
            zik.saveQuickPreset(name.text, album.text, albumEn.checked, artist.text, artistEn.checked, genre.text, genreEn.checked, title.text, titleEn.checked, false);
        }
    }
}
