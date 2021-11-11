import QtQuick 2.0

Item{
    id: thumbEqua

    property color fillColor: "grey"
    property color strokeColor: "black"
    property bool fill: true
    property bool stroke: true
    property int lineWidth: 3.0
    property int radius: height - lineWidth

    property real circleOpacity: 1.0
    property real textOpacity: 1.0

    property int r
    property int theta

    property bool pressed: false
    property bool xAnimRunning: false
    property bool yAnimRunning: false
    property bool animRunning: (xAnimRunning == false) && (yAnimRunning == false) ? false : true

    property real handleRadius: 10
    property color handleFillColor: "black"
    property color handleStrokeColor: "darkgrey"
    property real handleOpacity: 1.0

    property string imageSource: Qt.resolvedUrl("ThumbEqua.svg")

    property int paintedWidth: img.width
    property int paintedHeight: img.height

    Image{
        id: img
        anchors.centerIn: parent
        source: parent.imageSource

        sourceSize.width: parent.width + parent.width * 2/10
        sourceSize.height: parent.width + parent.width * 2/10

        opacity: parent.textOpacity
    }

    Rectangle{
        id: thumbCircle
        width: parent.width
        height: width
        radius: width/2

        color: parent.fillColor
        border.color: parent.stroke ? parent.strokeColor : "transparent"

        opacity: parent.circleOpacity

        MouseArea{

            preventStealing: true
            anchors.fill: parent

            function computeThumbValues(x,y){
                var xp = width / 2 - mouseX;
                var yp = height / 2 - mouseY;

                var a = Math.pow(xp, 2);
                var b = Math.pow(yp, 2);
                var c = Math.pow(width/2, 2);

                //if( a + b <= c ){

                    var r = Math.sqrt(a + b);
                    var theta = Math.acos( xp*-1 / r );
                    var theta_degree = 180 * theta / Math.PI;

                    if(yp < 0){
                        theta_degree = 180 - theta_degree + 180;
                    }

                    theta_degree = Math.floor(theta_degree);

                    r = r * 100 / (width / 2);
                    r = Math.floor(r);

                    if(r<=100){
                        thumbEqua.r = r;
                        thumbEqua.theta = theta_degree;

                        newHandle.x = mouseX - newHandle.width/2;
                        newHandle.y = mouseY - newHandle.height/2;
                    }else{
                        r=100
                        thumbEqua.r = r;
                        thumbEqua.theta = theta_degree;

                        newHandle.x = parent.width / 2 + (99*parent.width/2/100) * Math.cos((-theta_degree) * Math.PI / 180) - newHandle.width/2;
                        newHandle.y = parent.height / 2 + (99*parent.width/2/100) * Math.sin((-theta_degree) * Math.PI / 180) - newHandle.height/2;

                    }

                //}

            }


            onPositionChanged: {
                computeThumbValues(mouseX, mouseY);
            }

            onPressed: {
                thumbEqua.pressed = true;

                computeThumbValues(mouseX, mouseY);
            }
            onReleased: {
                thumbEqua.pressed = false;
            }

            onCanceled: {
            }

            onDoubleClicked: {
                thumbEqua.r = 1;
                thumbEqua.theta = 1;

                thumbEqua.pressed = false;

                newHandle.x = thumbEqua.width / 2 + (thumbEqua.r*thumbEqua.width/2/100) * Math.cos((-thumbEqua.theta) * Math.PI / 180) - newHandle.width/2
                newHandle.y = thumbEqua.height / 2 + (thumbEqua.r*thumbEqua.width/2/100) * Math.sin((-thumbEqua.theta) * Math.PI / 180) - newHandle.height/2
            }

        }

    }

    Rectangle {
        id: newHandle

        z:10

        width: handleRadius
        height: width
        radius: width/2

        color: parent.handleFillColor
        border.color: parent.handleStrokeColor

        opacity: parent.handleOpacity

        x: parent.width / 2 + (parent.r*parent.width/2/100) * Math.cos((-parent.theta) * Math.PI / 180) - width/2
        y: parent.height / 2 + (parent.r*parent.width/2/100) * Math.sin((-parent.theta) * Math.PI / 180) - height/2
    }


}


