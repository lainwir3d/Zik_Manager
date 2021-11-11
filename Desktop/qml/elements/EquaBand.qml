import QtQuick 2.0

Rectangle{
    id: band

    width: 2
    height: parent.height

    x: logposition(disp_freq)

    property real gain
    property int freq
    property real q: 1.0

    property real disp_gain: gain
    property int disp_freq: freq
    property real disp_q: q

    property int handleSize: 20
    property color handleColor: "white"

    property color textColor: "white"


    function logslider(position) {
        // position will be between 0 and 100
        var minp = 0+30;
        var maxp = paraEqua.width-30;

          // The result should be between 100 an 10000000
          var minv = Math.log(20);
        var maxv = Math.log(20000);

        // calculate adjustment factor
        var scale = (maxv-minv) / (maxp-minp);

        return Math.exp(minv + scale*(position-minp));
    }

    function logposition(value) {
        // position will be between 0 and 100
        var minp = 0+30;
        var maxp = paraEqua.width-30;

        // The result should be between 100 an 10000000
        var minv = Math.log(20);
        var maxv = Math.log(20000);

        // calculate adjustment factor
        var scale = (maxv-minv) / (maxp-minp);

        return (Math.log(value)-minv) / scale + minp;
    }

    function roundNearQtr(number){
        return (Math.round(number * 4) / 4).toFixed(2);
    }

    function roundNearInt(number, i){
        return Math.round(number / i) * i;
    }

    Timer {
        id: t
        interval: 50; running: false; repeat: true
        onTriggered: {
            parent.freq = parent.disp_freq;
            parent.gain = roundNearQtr(parent.disp_gain);
        }
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height / 2 - Math.round(parent.disp_gain*100/12.0)/100* parent.height/2 - height/2

        height: parent.handleSize
        width: parent.handleSize
        radius: width/2

        color: band.handleColor

        MouseArea{
            anchors.fill: parent


            onPositionChanged: {
                var screen_c = mapToItem(paraEqua, mouseX, mouseY)

                var nfreq = paraEqua.logslider(screen_c.x);

                if(nfreq < 20) nfreq = 20;
                if(nfreq > 20000) nfreq = 20000;
                parent.parent.disp_freq = nfreq;
                //parent.parent.freq = roundNearInt(nfreq, 100);


                var ngain = ((parent.parent.height / 2) - screen_c.y + 20 - parent.height/2) * 24 / parent.parent.height;
                if(ngain > 12) ngain = 12;
                if(ngain < -12) ngain = -12;

                parent.parent.disp_gain = ngain;
                //parent.parent.gain = roundNearQtr(ngain);

            }

            onReleased: {
                var screen_c = mapToItem(paraEqua, mouseX, mouseY)

                var nfreq = paraEqua.logslider(screen_c.x);

                if(nfreq < 20) nfreq = 20;
                if(nfreq > 20000) nfreq = 20000;
                parent.parent.disp_freq = nfreq;
                parent.parent.freq = nfreq;


                var ngain = ((parent.parent.height / 2) - screen_c.y + 20 - parent.height/2) * 24 / parent.parent.height;
                if(ngain > 12) ngain = 12;
                if(ngain < -12) ngain = -12;

                parent.parent.disp_gain = ngain;
                parent.parent.gain = roundNearQtr(ngain);

                t.stop();
            }

            onPressed: {
                t.start();
            }
        }
        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.right
            text: roundNearQtr(parent.parent.disp_gain)

            color: band.textColor
        }

   }
   Text{
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.top: parent.bottom
       text: parent.disp_freq

       color: band.textColor
   }
}
