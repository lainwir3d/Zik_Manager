import QtQuick 2.0

Rectangle {
    id: paraEqua

    color: "grey"

    property int handleSize: 20

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

    function updateEqua(){
        zik.setParametricEqualizer(band1.freq, band2.freq, band3.freq, band4.freq, band5.freq, band1.q.toFixed(2), band2.q.toFixed(2), band3.q.toFixed(2), band4.q.toFixed(2), band5.q.toFixed(2), band1.gain.toFixed(2), band2.gain.toFixed(2), band3.gain.toFixed(2), band4.gain.toFixed(2), band5.gain.toFixed(2));
    }

    Component.onCompleted: {
        updateEqua();
    }

    EquaBand{
        id: band1

        anchors.verticalCenter: parent.verticalCenter

        gain: 0
        freq: 31
        //freq: logslider(parent.width/6 *1)

        onGainChanged: updateEqua()
        onFreqChanged: updateEqua()

        handleSize: parent.handleSize
    }

    EquaBand{
        id: band2

        anchors.verticalCenter: parent.verticalCenter

        gain: 0
        freq: 140
        //freq: logslider(parent.width/6 *2)

        onGainChanged: updateEqua()
        onFreqChanged: updateEqua()

        handleSize: parent.handleSize
    }

    EquaBand{
        id: band3

        anchors.verticalCenter: parent.verticalCenter

        gain: 0
        freq: 632
        //freq: logslider(parent.width/6 *3)

        onGainChanged: updateEqua()
        onFreqChanged: updateEqua()


        handleSize: parent.handleSize
    }

    EquaBand{
        id: band4

        anchors.verticalCenter: parent.verticalCenter

        gain: 0
        freq: 2850
        //freq: logslider(parent.width/6 *4)

        onGainChanged: updateEqua()
        onFreqChanged: updateEqua()


        handleSize: parent.handleSize
    }

    EquaBand{
        id: band5

        anchors.verticalCenter: parent.verticalCenter

        gain: 0
        freq: 12844
        //freq: logslider(parent.width/6 *5)

        onGainChanged: updateEqua()
        onFreqChanged: updateEqua()

        handleSize: parent.handleSize
    }

    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        z:2

        width: parent.width
        height: 2

        color: "white"
    }

    Rectangle{
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left

        width: 2
        height: parent.height

        color: "white"
    }
}
