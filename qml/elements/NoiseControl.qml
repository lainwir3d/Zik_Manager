import QtQuick 2.0

Item{
    id: noiseControl

    property color borderColor: "black"
    property color activeBorderColor: "orange"
    property color fillColor: "transparent"
    property color activeFillColor: "white"
    property color handlesColor: "black"

    property int fixedRandomNoiseCoeff: 50

    property int handleSize: 20

    property bool autoNc: false

    Connections {
        target: zik
        onNcTypeChanged: {
            animateExternalChange();
        }
        onNcValueChanged: {
            animateExternalChange();
        }
    }

    function animateExternalChange() {
        animateControlCircle.from = controlCircle.width;
        animateControlCircle.to = controlCircle.calculateWidth();
        animateControlCircle.start();
    }

    Rectangle {
        id: aocCircle

        anchors.centerIn: parent

        width: parent.width
        height: width
        radius: width/2
        color: "transparent"
        border.color: parent.borderColor

        visible: false
    }
    Rectangle {
        id: aocQuietCircle

        anchors.centerIn: parent

        width: parent.width * 0.75 + (aocCircle.width - parent.width * 0.75)/2
        height: width
        radius: width/2
        color: "transparent"
        border.color: parent.borderColor

        visible: false
    }

    Rectangle {
        id: offCircle

        anchors.centerIn: parent

        width: aocCircle.width * 0.75
        height: width
        radius: width/2
        color: "transparent"
        border.color: noiseControl.borderColor

        visible: false
    }
    Rectangle {
        id: ancQuietCircle

        anchors.centerIn: parent

        width: ancCircle.width + (offCircle.width - ancCircle.width)/2
        height: width
        radius: width/2
        color: "transparent"
        border.color: noiseControl.borderColor

        visible: false
    }
    Rectangle {
        id: ancCircle

        anchors.centerIn: parent

        width: aocCircle.width * 0.5
        height: width
        radius: width/2
        color: "transparent"
        border.color: noiseControl.borderColor
    }

    Item{
        Repeater{
            //model:8
            model:4
            Rectangle {
                x: noiseControl.width / 2 + ancCircle.width / 2 * Math.cos(0 * Math.PI / 180) - width/2 + (aocCircle.width-ancCircle.width)/8 * index
                y: noiseControl.height / 2 + ancCircle.width / 2 * Math.sin(0 * Math.PI / 180) - height/2
                height: 3
                //width:(aocCircle.width-ancCircle.width)/40
                width: 3
                radius: width/2
                color: "white"

                opacity: autoNc ? 0.0 : 1.0

                Behavior on opacity { NumberAnimation{ duration: 500 } }
            }
        }
    }
    Rectangle {
        x: parent.width / 2 + aocCircle.width / 2 * Math.cos(0 * Math.PI / 180) - width/2
        y: parent.height / 2 + aocCircle.width / 2 * Math.sin(0 * Math.PI / 180) - height/2
        //height:(aocCircle.width-ancCircle.width)/20
        height:5
        width:5
        radius:width/2
        color:"white"
    }
    Rectangle {
        x: parent.width / 2 + offCircle.width / 2 * Math.cos(0 * Math.PI / 180) - width/2
        y: parent.height / 2 + offCircle.width / 2 * Math.sin(0 * Math.PI / 180) - height/2
        //height:(aocCircle.width-ancCircle.width)/20
        height:5
        width:5
        radius:width/2
        color:"white"
    }
    Rectangle {
        x: parent.width / 2 + ancCircle.width / 2 * Math.cos(0 * Math.PI / 180) - width/2
        y: parent.height / 2 + ancCircle.width / 2 * Math.sin(0 * Math.PI / 180) - height/2
        //height:(aocCircle.width-ancCircle.width)/20
        height:5
        width:5
        radius:width/2
        color:"white"
    }



    Rectangle {
        id: controlCircle

        anchors.centerIn: parent

        width: calculateWidth()
        height: width
        radius: width/2
        color: "transparent"
        border.color: noiseControl.activeBorderColor

        Connections{
            target: noiseControl
            onWidthChanged: {
                controlCircle.width = controlCircle.calculateWidth();
            }

        }

        Component.onCompleted: {
            // Force unbind or the animation get screwed until we change the NC manually
            controlCircle.width = controlCircle.calculateWidth();
        }

        function calculateWidth(){
            var tmp;
            //zik.ncType == "anc" ? ancCircle.width : zik.ncType == "off" ? offCircle.width : aocCircle.width;

            if((zik.ncType == "anc") && (zik.ncValue == 2)) tmp = ancCircle.width;
            else if((zik.ncType == "anc") && (zik.ncValue == 1)) tmp = ancQuietCircle.width;
            else if(zik.ncType == "off") tmp = offCircle.width;
            else if((zik.ncType == "aoc") && (zik.ncValue == 1)) tmp = aocQuietCircle.width;
            else if((zik.ncType == "aoc") && (zik.ncValue == 2)) tmp = aocCircle.width;
            else tmp = offCircle.width;

            return tmp;
        }


       Rectangle {
            id: controlHandle
            visible: true

            width: noiseControl.handleSize * 2
            height: width
            radius: width / 2
            color: "transparent"

            x: parent.width / 2 + controlCircle.width / 2 * Math.cos(0 * Math.PI / 180) - width/2
            y: parent.height / 2 + controlCircle.width / 2 * Math.sin(0 * Math.PI / 180) - height/2

            Rectangle {
                id: controlHandleDecorator

                width: noiseControl.handleSize
                height: width
                radius: width / 2
                color: noiseControl.handlesColor

                anchors.centerIn: parent
            }

            MouseArea {
                anchors.centerIn: parent.Center

                width: parent.width
                height: parent.height

                preventStealing: true

                onPositionChanged: {
                    var screen_c = mapToItem(noiseControl, mouseX, mouseY)

                    var newWidth = screen_c.x - (noiseControl.width - controlCircle.width) / 2

                    if(newWidth < ancCircle.width) newWidth = ancCircle.width;
                    if(newWidth > aocCircle.width) newWidth = aocCircle.width;

                    controlCircle.width = newWidth;

                }

                onReleased: {
                    var d_anc = Math.abs(ancCircle.width - controlCircle.width);
                    var d_ancQuiet = Math.abs(ancQuietCircle.width - controlCircle.width);
                    var d_off = Math.abs(offCircle.width - controlCircle.width);
                    var d_aoc = Math.abs(aocCircle.width - controlCircle.width);
                    var d_aocQuiet = Math.abs(aocQuietCircle.width - controlCircle.width);


                    var type;
                    var value;

                    var min;
                    if(autoNc){
                        min = Math.min(d_anc, d_off, d_aoc);
                    }else{
                        min = Math.min(d_anc, d_ancQuiet, d_off, d_aocQuiet, d_aoc);
                    }

                    animateControlCircle.from = controlCircle.width;
                    if(min == d_anc){
                        //console.log("ANC Transport.");
                        animateControlCircle.to = ancCircle.width;

                        type = "anc";
                        value = 2;

                    }else if(min == d_ancQuiet){
                        //console.log("ANC Quiet.");
                        animateControlCircle.to = ancQuietCircle.width;

                        type = "anc";
                        value = 1;
                    }else if(min == d_off){
                        //console.log("NC off.");
                        animateControlCircle.to = offCircle.width;

                        type = "off";
                        value = 0;
                    }else if(min == d_aocQuiet){
                        //console.log("AOC Quiet.");
                        animateControlCircle.to = aocQuietCircle.width;

                        type = "aoc";
                        value = 1;
                    }else if(min == d_aoc){
                        //console.log("AOC Transport.");
                        animateControlCircle.to = aocCircle.width;

                        type = "aoc";
                        value = 2;
                    }

                    //zik.ncType = type;
                    zik.setAoc(type, value);
                    animateControlCircle.start();

                }


            }

        }
    }



    NumberAnimation {
         id: animateControlCircle
         target: controlCircle
         properties: "width"
         from: controlCircle.width
         to: controlCircle.width
         easing {type: Easing.Linear; }
         duration: 150
    }


    Repeater{
        model:346/4
        Rectangle {
            id:noise

            smooth: noiseControl.smooth
            antialiasing: noiseControl.antialiasing

            z:-3
            width: 1
            height: Math.PI * noiseControl.width / 346
            color: "grey"
            opacity: 1  // performance !!

            transform: Rotation{
                angle: index*4+8
                origin.x: 0
                origin.y: height / 2
            }

            x: parent.width / 2 + (ancCircle.width+3) / 2 * Math.cos((index*4+8) * Math.PI / 180)
            y: parent.height / 2 + (ancCircle.width+3) / 2 * Math.sin((index*4+8) * Math.PI / 180) - height/2

            NumberAnimation {
                id: animNoise
                target: noise;
                property: "width";
                duration: 1000;
                easing.type: Easing.InQuad

                running: Qt.application.active

                property bool first: true

                from: 50
                to: 0

                onStopped: {
                    animNoise.from = width;
                    animNoise.to = zik.generateRandom(3* ancCircle.radius >> 3, ancCircle.radius);


                    if(first){
                        animTimer.start();
                        first = false;
                    }else{
                        if(Qt.application.active) animNoise.start();
                    }
                }


            }

            Timer {
                id: animTimer
                interval: 1000

                running: Qt.application.active

                onTriggered: {
                    animNoise.duration = 100;
                    animNoise.easing.type = Easing.Linear;
                    animNoise.start();

                }
            }

            Component.onCompleted: {
                animNoise.start();
            }
        }
    }

}
