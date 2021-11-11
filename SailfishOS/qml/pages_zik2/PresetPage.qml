import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: firstPage

    SilicaListView{
        id: listView
        anchors.fill: parent

        flickableDirection: Flickable.VerticalFlick

        property Item contextMenu

        header: PageHeader {
            title: "Presets"
        }

        PullDownMenu {
            MenuItem {
                text: "Quick Preset"
                onClicked: pageStack.push(Qt.resolvedUrl("QuickPresetPage.qml"))
            }
            MenuItem {
                id: smartAudioTune
                text: "Smart Audio Tune"
                font.bold: zik.smartAudioTune == "true" ? true : false
                onClicked: {
                     if(zik.smartAudioTune == "true"){
                        zik.smartAudioTune = "false";
                    }else{
                        zik.smartAudioTune = "true";
                    }
                }
           }
        }

        model: presetModel
        delegate: Item{
            id: myListItem
            property bool menuOpen: listView.contextMenu != null && listView.contextMenu.parent === myListItem

            width: ListView.view.width
            height: menuOpen ? listView.contextMenu.height + contentItem.height : contentItem.height

            BackgroundItem {
                id: contentItem
                width: parent.width

                property var rem: remorse
                TextSwitch{
                    text: name

                    property bool status : p_enabled
                    property bool menuOpen: listView.contextMenu != null && listView.contextMenu.parent === myListItem

                    Component.onCompleted: checked = p_enabled

                    onCheckedChanged: {
                        if(checked != p_enabled){
                            zik.enablePreset(id, checked);
                        }
                    }

                    onStatusChanged: {
                        checked = p_enabled
                    }

                    onPressAndHold: {
                        if (!listView.contextMenu)
                            listView.contextMenu = contextMenuComponent.createObject(listView)
                        listView.contextMenu.presetId = id;
                        listView.contextMenu.item = contentItem;
                        listView.contextMenu.show(myListItem)
                    }
                }
                function remove() {
                    remorse.execute(contentItem, "Deleting", function() { zik.deletePreset(id) } )
                }
               RemorseItem { id: remorse }
            }
        }


        Component {
            id: contextMenuComponent
            ContextMenu {
                id: ctxmenu
                property int presetId;
                property var item;
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        ctxmenu.item.remove();
                        listView.scrollToTop();	// Provokes an UI bug.
                    }
                }
            }
        }
    }

}
