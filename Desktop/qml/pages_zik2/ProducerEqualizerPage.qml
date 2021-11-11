import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import "../elements"
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

         text: "Next"
         textColor: "white"

         onClicked: {
             firstPage.Stack.view.push(Qt.resolvedUrl("SavePresetPage.qml"));
         }
    }

    LabeledSwitch {
        anchors.top: backButton.bottom
        anchors.left: parent.left
        anchors.margins: 5
        anchors.horizontalCenter: parent.horizontalCenter

        text: "Bypass"
        textColor: "white"

        height: 40

        switchCheckedColor: "orange"
        switchUncheckedColor: "white"

        checked: zik.presetBypass == "true" ? true : false
        onCheckedChanged: {
            if(checked){
                zik.presetBypass = "true"
            }else{
                zik.presetBypass = "false"
            }
        }
    }

    ParametricEqualizer{
        id: paramEq
        anchors.centerIn: parent

        width: parent.width - 20
        height: parent.height / 2
        opacity: 0.5

        onWidthChanged: {
            //console.log("fait chier");
        }

        onHeightChanged: {
            //console.log("pareil");
        }

        handleSize: 15

    }

}
