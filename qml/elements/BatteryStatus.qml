import QtQuick 2.2

Item {
    id: batteryStatus

    property int percent
    property int timeleft

    property int percent_anim: 100

    property int unitWidth: width * 1/10
    property int margin: 10

    property color textColor: "white"

    property bool debug: false

    property string batIconSource: "bolt.svg"

    property string fontFamily: "Liberation Sans"

    onPercentChanged: {
        timer.start();
    }

    Timer{
        id: timer
        interval: 500
        repeat: false
        onTriggered: {
            batteryStatus.percent_anim = batteryStatus.percent;
        }
    }

    Behavior on percent_anim { NumberAnimation { duration: 1000; easing.type: Easing.InOutQuad }}

    Text {
        id: refText
        visible: false

        anchors.centerIn: parent

        width: parent.width - unitWidth*2 - margin*4

        font.pixelSize: width / 2
        font.family: fontFamily

        text: "100"
    }

    Text {
        id: percentText

        height: percentText.paintedHeight

        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: timeleft != -1 ? timeleft+"m" : percent_anim != -1 ? percent_anim : "?"
        color: parent.textColor


        font.pixelSize: refText.font.pixelSize
        font.family: fontFamily
    }

    Image {
        id: battImg

        anchors.bottom: percentText.top
        anchors.margins: 0

        anchors.horizontalCenter: percentText.horizontalCenter

        source: batIconSource

        sourceSize.height: unitTxt.paintedHeight
        sourceSize.width: unitTxt.paintedHeight

        height: unitTxt.paintedHeight
        fillMode: Image.PreserveAspectFit

        opacity: 0.0
        smooth: true

        SequentialAnimation {
            running: zik.batteryState == "charging"
            loops: -1
            NumberAnimation { target: battImg; property: "opacity"; to: 0.5; duration: 500 }
            NumberAnimation { target: battImg; property: "opacity"; to: 1.0; duration: 500 }
        }

        SequentialAnimation {
            running: zik.batteryState == "charged"
            NumberAnimation { target: battImg; property: "opacity"; to: 1.0; duration: 500 }
        }

        SequentialAnimation {
            running: zik.batteryState == "in_use"
            NumberAnimation { target: battImg; property: "opacity"; to: 0.0; duration: 500 }
        }
    }


    Text {
        id: unitTxt

        anchors.top: percentText.bottom
        anchors.margins: 0

        anchors.horizontalCenter: percentText.horizontalCenter

        text: "%"
        color: parent.textColor

        font.pixelSize: percentText.font.pixelSize / 3
        font.family: fontFamily
    }

    Rectangle {
        anchors.centerIn: parent

        width:2
        height:2

        visible: parent.debug
    }

    Repeater{
        model:360/4
        Rectangle {
            id:noise

            smooth: true
            antialiasing: true

            property int highlightValue: percent_anim

            z:-3
            width: batteryStatus.unitWidth// + index
            height: 2
            color: highlightValue == 100 ? "white" : highlightValue == -1 ? "grey" : (index * 100 / (360/4)) <= (100 - highlightValue) ? "grey" : "white"
            //opacity:0.5

            transform: Rotation{
                angle: 270+index*4
                origin.x: 0
                origin.y: 0
            }

            x: batteryStatus.width / 2 + (batteryStatus.width - batteryStatus.unitWidth*2 - batteryStatus.margin*2) / 2 * Math.cos((270+index*4) * Math.PI / 180)
            y: batteryStatus.height / 2 + (batteryStatus.width - batteryStatus.unitWidth*2 - batteryStatus.margin*2) / 2 * Math.sin((270+index*4) * Math.PI / 180) - height/2


        }
    }
}
