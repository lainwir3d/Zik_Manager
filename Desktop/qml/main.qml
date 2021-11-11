import QtQuick 2.2
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

import QtQuick.Window 2.2

import ZikManager 1.0

import "pages_zik2/"

Item {
    id: appWindow

    anchors.fill: parent

    property real inches: Math.sqrt( Math.pow(width / Screen.pixelDensity, 2) +  Math.pow(height / Screen.pixelDensity, 2)) / 25.4
    property real ratio: width/height
    property bool isWidescreen: (ratio > 1.0)
    property bool isBigScreen: (inches >= 7)
    property bool isFullview: (isBigScreen && isWidescreen) ? true : false

    property bool isPortrait: Screen.primaryOrientation === Qt.PortraitOrientation || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    /*
    onIsWidescreenChanged: {
      determineUI();
    }

    onIsBigScreenChanged: {
      determineUI();
    }
    */

    // This should be enough
    onIsFullviewChanged: {
      //print("inches:" +inches);
      determineUI();
    }

    function determineUI(){
        if(zik.isConnected()){
            if(isFullview){
                if(zik.model == "ZIK1"){
                    // no widescreen support for zik 1
                }else{
                    stackView.replace(Qt.resolvedUrl("pages_zik2/widescreen.qml"));
                }
            }else if(isBigScreen){
                if(zik.model == "ZIK1"){
                    // no need to switch back for zik 1 since we never went widescreen in the first place
                }else{
                    stackView.replace(Qt.resolvedUrl("pages_zik2/FirstPage.qml"));
                }
            }else{
                // nothing to do since we never went fullview
            }
        }
    }

    /*
    Keys.onPressed: {
        accepted = false;
    }
    */

    Connections {
             target: zik
             onConnected: {
                 if(stackView.depth == 1){
                    var page;
                    if(isFullview){
                        page = (zik.model == "ZIK1") ? "pages_zik2/FirstPage.qml" : "pages_zik2/widescreen.qml";
                    }else{
                        page = "pages_zik2/FirstPage.qml";
                    }
                    stackView.replace(Qt.resolvedUrl(page));
                 }else{
                    stackView.pop();
                 }
             }
             onDisconnected: {
                 stackView.replace(Qt.resolvedUrl("pages_zik2/LoadingPage.qml"));
             }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: Qt.resolvedUrl("pages_zik2/LoadingPage.qml")

        delegate: StackViewDelegate {
            function getTransition(properties)
            {
                var transitionName = properties.enterItem.transitionName ? properties.enterItem.transitionName : properties.exitItem.transitionName ? properties.exitItem.transitionName : properties.name;
                return this[transitionName];
            }

            function transitionFinished(properties)
            {
                properties.exitItem.x = 0;
                properties.exitItem.y = 0;

                properties.exitItem.opacity = 1
            }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "x"
                    from: target.width
                    to: 0
                    duration: 300
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: -target.width
                    duration: 300
                }
            }

            popTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "x"
                    from: -target.width
                    to: 0
                    duration: 300
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: target.width
                    duration: 300
                }
            }

            replaceTransition: fadeTransition

            property Component fadeTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: target.opacity
                    duration: 300
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: target.opacity
                    to: 0
                    duration: 300
                }
            }
        }
    }

    Image {
        id: background
        anchors.fill: parent
        z:-1

        source: Qt.resolvedUrl("pages_zik2/bg.jpg")

        visible: false
    }

    FastBlur{
        anchors.fill: background
        source: background
        radius: 64

        z:-1

        MouseArea {
            anchors.fill: parent

            acceptedButtons: Qt.RightButton

            onClicked: {
                if(stackView.depth > 1){
                    stackView.pop();
                }
            }
        }
    }
}
