import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import "pages_zik2/"

Item {
    id: appWindow

    anchors.fill: parent

    TextArea {
        id: codeArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bp.top
        anchors.top: parent.top
    }

    Button {
        id: bp

        text: "exec"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onClicked: {
            var newObject = Qt.createQmlObject(codeArea.text,
                                               appWindow,
                                               "dynamicSnippet1");
            zik.autoNC
        }
    }
}

