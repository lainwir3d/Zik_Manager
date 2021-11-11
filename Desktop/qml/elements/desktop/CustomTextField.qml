import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import QtGraphicalEffects 1.0

Item {

    height: lab.height + f.height + sep.height
    width: ref.paintedWidth

    property alias field: f
    property alias text: f.text
    property alias textColor: f.textColor
    property alias readOnly: f.readOnly

    property string placeholder: ""
    property string label: ""

    Text {
        id: ref
        visible: false
        text: f.text.length ? f.text : f.placeholderText
    }

    TextField {
        id: f
        textColor: "white"

        anchors.top: parent.top
        anchors.margins: 0
        width: ref.paintedWidth + 10
        height: ref.paintedHeight + 10

        placeholderText: parent.label + ": " + parent.placeholder

        style: TextFieldStyle{
            padding.left: 0
            background: Item { }
            placeholderTextColor: "lightgrey"
        }
    }

    CustomSeparator {
        id: sep
        anchors.top: f.bottom
        anchors.margins: -4
        width: ref.paintedWidth
        height: visible ? 1 : 0

        visible: !readOnly
    }

    Text {
        id: lab
        anchors.top: sep.bottom
        anchors.topMargin: 0

        visible: f.text.length != 0

        text: parent.label
        color: f.textColor

        opacity: 0.5
    }
}
