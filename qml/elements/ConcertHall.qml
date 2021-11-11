import QtQuick 2.0

Item{
    id: concertHall

    property color borderColor: "black"
    property color activeBorderColor: "white"
    property color fillColor: "transparent"
    property color activeFillColor: "white"
    property real circleOpacity: 1
    property bool superposeColors: false

    property color handlesColor: "black"
    property color handlesDisabledColor: "grey"
    property string handleImage: ""
    property real handleOpacity: 1
    property int handleDiameter: 40

    property color textColor: "white"
    property real textOpacity:1
    property bool textVisible: true

    property bool surround
    property string room
    property int angle

    property int animationAngle: surround ? 30 : angle  // at init, reassigned in Component.onCompleted

    property int currentCircleWidth


    signal silentClicked()
    signal livingClicked()
    signal jazzClicked()
    signal concertClicked()

    onSurroundChanged: {
        if(surround){
            animateAngle.from = concertHall.angle;
            animateAngle.to = 30;

        }else{
            animateAngle.from = 30;
            animateAngle.to = concertHall.angle;
        }

        animateAngle.start();
    }

    Component.onCompleted: {
        if(room == "concert"){
            concertClicked();
            currentCircleWidth = concertCircle.width;
        }else if(room == "jazz"){
            jazzClicked();
            currentCircleWidth = jazzCircle.width
        }else if(room == "living"){
            livingClicked();
            currentCircleWidth = livingCircle.width
        }else if(room == "silent"){
            silentClicked();
            currentCircleWidth = silentCircle.width
        }

        animationAngle = animationAngle;  // unbind from surround dependence
    }

    onSilentClicked: {
        if(superposeColors){
            concertCircle.color = fillColor;
            jazzCircle.color = fillColor;
            livingCircle.color = fillColor;
            silentCircle.color = activeFillColor;
        }else{
            concertCircle.color = fillColor;
            jazzCircle.color = fillColor;
            livingCircle.color = fillColor;
            silentCircle.color = activeFillColor;
        }

        currentCircleWidth = silentCircle.width;
    }

    onLivingClicked: {
        if(superposeColors){
            concertCircle.color = fillColor;
            jazzCircle.color = fillColor;
            livingCircle.color = activeFillColor;
            silentCircle.color = activeFillColor;
        }else{
            concertCircle.color = fillColor;
            jazzCircle.color = fillColor;
            livingCircle.color = activeFillColor;
            silentCircle.color = fillColor;
        }

        currentCircleWidth = livingCircle.width;
    }

    onJazzClicked: {
        if(superposeColors){
            concertCircle.color = fillColor;
            jazzCircle.color = activeFillColor;
            livingCircle.color = activeFillColor;
            silentCircle.color = activeFillColor;
        }else{
            concertCircle.color = fillColor;
            jazzCircle.color = activeFillColor;
            livingCircle.color = fillColor;
            silentCircle.color = fillColor;
        }

        currentCircleWidth = jazzCircle.width;
    }

    onConcertClicked: {
        if(superposeColors){
            concertCircle.color = activeFillColor;
            jazzCircle.color = activeFillColor;
            livingCircle.color = activeFillColor;
            silentCircle.color = activeFillColor;
        }else{
            concertCircle.color = activeFillColor;
            jazzCircle.color = fillColor;
            livingCircle.color = fillColor;
            silentCircle.color = fillColor;
        }

        currentCircleWidth = concertCircle.width;
    }

    Rectangle {
        id: concertCircle
        anchors.centerIn: parent
        width: parent.width
        height: width
        radius: width/2
        color: parent.fillColor
        border.color: parent.borderColor

        opacity: circleOpacity

        MouseArea {
            anchors.fill: parent

            onClicked: {
                concertHall.concertClicked();
                concertHall.room = "concert";
            }
        }
    }

    Rectangle {
        id: jazzCircle

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        width: concertCircle.width * 0.75
        height: width
        radius: width/2
        color: concertHall.fillColor
        border.color: concertHall.borderColor

        opacity: circleOpacity

        MouseArea {
            anchors.fill: parent

            onClicked: {
                concertHall.jazzClicked();
                concertHall.room = "jazz";
            }
        }
    }

    Rectangle {
        id: livingCircle

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        width: concertCircle.width * 0.5
        height: width
        radius: width/2
        color: concertHall.fillColor
        border.color: concertHall.borderColor

        opacity: circleOpacity

        MouseArea {
            anchors.fill: parent

            onClicked: {
                concertHall.livingClicked();
                concertHall.room = "living";
            }

        }

    }


    Rectangle {
        id: silentCircle

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        width: concertCircle.width * 0.25
        height: width
        radius: width/2
        color: concertHall.fillColor
        border.color: concertHall.borderColor

        opacity: circleOpacity

        MouseArea {
            anchors.fill: parent

            onClicked: {
                concertHall.silentClicked();
                concertHall.room = "silent";
            }
        }
    }


    Text {
        id: concertCircleText
        text: qsTr("Concert")

        anchors.horizontalCenter: parent.horizontalCenter
        y: concertCircle.y + (concertCircle.height - jazzCircle.height) / 2 - font.pixelSize / 2

        color: concertHall.textColor
        opacity: concertHall.textOpacity

        font.pixelSize: silentCircleText.font.pixelSize
        visible: concertHall.textVisible
    }

    Text {
        id: jazzCircleText
        text: qsTr("Jazz")

        anchors.horizontalCenter: parent.horizontalCenter
        y: jazzCircle.y + (jazzCircle.height - livingCircle.height) / 2 - font.pixelSize / 2

        color: concertHall.textColor
        opacity: concertHall.textOpacity

        font.pixelSize: silentCircleText.font.pixelSize
        visible: concertHall.textVisible
    }

    Text {
        id: livingCircleText
        text: qsTr("Living")

        anchors.horizontalCenter: parent.horizontalCenter
        y: livingCircle.y + (livingCircle.height - silentCircle.height) / 2 - font.pixelSize / 2

        color: concertHall.textColor
        opacity: concertHall.textOpacity

        font.pixelSize: silentCircleText.font.pixelSize
        visible: concertHall.textVisible
    }

    Text {
        id: silentCircleText
        text: qsTr("Silent")

        anchors.horizontalCenter: parent.horizontalCenter
        y: silentCircle.y + silentCircle.height/ 2 - font.pixelSize / 2

        color: concertHall.textColor
        opacity: concertHall.textOpacity

        font.pixelSize: silentCircle.width / 4
        visible: concertHall.textVisible
    }


    Rectangle {
        width: parent.handleDiameter
        height: width
        radius: width / 2
        color: surround ? parent.handlesDisabledColor : parent.handlesColor

        x: parent.width / 2 + concertCircle.width / 2 * Math.cos((-105 + parent.animationAngle) * Math.PI / 180) - width/2
        y: parent.height / 2 + concertCircle.width / 2 * Math.sin((-105 + parent.animationAngle) * Math.PI / 180) - height/2

        opacity: handleOpacity

        Behavior on color { ColorAnimation { duration: 500 } }

        Image{
            anchors.fill: parent
            source: concertHall.handleImage

            rotation: 180
        }

        MouseArea {
            anchors.fill: parent
            preventStealing: true

            onPositionChanged: {

                if(!surround){
                    var screen_c = mapToItem(concertHall, mouseX, mouseY)

                    var p_angle =  Math.atan2(screen_c.y - concertHall.height/2, screen_c.x - concertHall.width/2);
                    p_angle = p_angle*180/Math.PI;

                    if(p_angle < 0){
                        p_angle = -p_angle;
                    }else{
                        p_angle = 360 - p_angle;
                    }

                    if(p_angle <= 75){
                        concertHall.animationAngle = 105 - p_angle;
                    }else if(p_angle >= 285){
                        concertHall.animationAngle = 105 + (360 - p_angle);
                    }
                }
            }

            onReleased: {

                // accept event but do nothing in surround
                if(!surround){
                    animateAngle.from = concertHall.animationAngle

                    if(concertHall.animationAngle <= 45){
                        animateAngle.to = 30;
                    }else if(concertHall.animationAngle <= 60){
                        animateAngle.to = 60;
                    }else if(concertHall.animationAngle <= 75){
                        animateAngle.to = 60;
                    }else if(concertHall.animationAngle <= 90){
                        animateAngle.to = 90;
                    }else if(concertHall.animationAngle <= 105){
                        animateAngle.to = 90;
                    }else if(concertHall.animationAngle <= 120){
                        animateAngle.to = 120;
                    }else if(concertHall.animationAngle <= 135){
                        animateAngle.to = 120;
                    }else if(concertHall.animationAngle <= 150){
                        animateAngle.to = 150;
                    }else if(concertHall.animationAngle <= 165){
                        animateAngle.to = 150;
                    }else if(concertHall.animationAngle <= 180){
                        animateAngle.to = 180;
                    }

                    concertHall.angle = animateAngle.to;
                    animateAngle.start();
                }
            }


        }

    }

    Rectangle {

        width: parent.handleDiameter
        height: width
        radius: width / 2
        color: surround ? parent.handlesDisabledColor : parent.handlesColor

        x: parent.width / 2 + concertCircle.width / 2 * Math.cos((-75 - parent.animationAngle) * Math.PI / 180) - width/2
        y: parent.height / 2 + concertCircle.width / 2 * Math.sin((-75 - parent.animationAngle) * Math.PI / 180) - height/2

        property point listenerCenter: Qt.point(silentCircle.x + silentCircle.width/2, silentCircle.y + silentCircle.y/2)
        property point speakerCenter: Qt.point(x + width/2, y+height/2)

        opacity: handleOpacity

        Behavior on color { ColorAnimation { duration: 500 } }

        Image{
            anchors.fill: parent
            source: concertHall.handleImage
        }


        MouseArea {
            anchors.fill: parent
            preventStealing: true

            onPositionChanged: {

                if(!surround){
                    var screen_c = mapToItem(concertHall, mouseX, mouseY)

                    var p_angle =  Math.atan2(screen_c.y - concertHall.height/2, screen_c.x - concertHall.width/2);
                    p_angle = p_angle*180/Math.PI;

                    if(p_angle < 0){
                        p_angle = -p_angle;
                    }else{
                        p_angle = 360 - p_angle;
                    }

                    if((p_angle >= 105) && (p_angle <= 255)){
                        concertHall.animationAngle = p_angle - 75;
                    }
                }
            }

            onReleased: {

                // accept event but do nothing in surround
                if(!surround){
                    animateAngle.from = concertHall.animationAngle

                    if(concertHall.animationAngle <= 45){
                        animateAngle.to = 30;
                    }else if(concertHall.animationAngle <= 60){
                        animateAngle.to = 60;
                    }else if(concertHall.animationAngle <= 75){
                        animateAngle.to = 60;
                    }else if(concertHall.animationAngle <= 90){
                        animateAngle.to = 90;
                    }else if(concertHall.animationAngle <= 105){
                        animateAngle.to = 90;
                    }else if(concertHall.animationAngle <= 120){
                        animateAngle.to = 120;
                    }else if(concertHall.animationAngle <= 135){
                        animateAngle.to = 120;
                    }else if(concertHall.animationAngle <= 150){
                        animateAngle.to = 150;
                    }else if(concertHall.animationAngle <= 165){
                        animateAngle.to = 150;
                    }else if(concertHall.animationAngle <= 180){
                        animateAngle.to = 180;
                    }

                    concertHall.angle = animateAngle.to;
                    animateAngle.start();
                }
            }
        }

    }

    NumberAnimation {
         id: animateAngle
         target: concertHall
         properties: "animationAngle"
         from: concertHall.animationAngle
         to: concertHall.angle
         easing {type: Easing.Linear; }
         duration: 250
    }

}
