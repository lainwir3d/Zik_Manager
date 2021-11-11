import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Item{
    height: area.contentHeight + description.contentHeight + 2

    property string text: ""
    property string label: ""
    property bool readOnly: false

    TextArea {
        id: area
        enabled: true
        width: parent.width
        height: contentHeight
        readOnly: parent.readOnly

        backgroundVisible: false
        frameVisible: false
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff

        text: parent.text

        style: TextAreaStyle {
            textColor: "white"
            selectionColor: "transparent"
            selectedTextColor: "white"
            backgroundColor: "transparent"
            textMargin: 0
            font: description.font
        }
    }
    Text {
        id: description
        anchors.top: area.bottom
        anchors.topMargin: 2
        anchors.left: parent.left
        text: parent.label
        color: "white"
        opacity: 0.5
    }
}
