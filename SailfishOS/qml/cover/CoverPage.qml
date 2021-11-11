/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import ZikManager 1.0
import "qrc:/elements"

CoverBackground {
    id: cover

    Label {
        id: label
        anchors.top: parent.top
        anchors.margins: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: zik.model == "ZIK1" ? "Zik 1" : zik.model == "ZIK2" ? " Zik 2.0" : zik.model == "ZIK3" ? "Zik 3" : "Zik"
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeLarge
    }

    Connections {
        target: zik
        onConnected: {
            loader.sourceComponent = batt;
            coverAction.enabled = true;
        }
        onDisconnected: {
            loader.sourceComponent = load;
            coverAction.enabled = false;
        }
    }

    Connections{
        target:  Qt.application
        onActiveChanged : {
            var visible = Qt.application.active | (cover.status == Cover.Active);

            if(visible){
                if(nonVisible.running) nonVisible.stop();
                zik.setApplicationVisible(true);
            }else{
                nonVisible.start()
            }
        }
    }

    Connections{
        target: cover
        onStatusChanged : {
            var visible = Qt.application.active | (cover.status == Cover.Active);

            if(visible){
                if(nonVisible.running) nonVisible.stop();
                zik.setApplicationVisible(true);
            }else{
                nonVisible.start()
            }
        }
    }

    Timer {
        id: nonVisible
        interval: 500
        repeat: false
        onTriggered: {
            zik.setApplicationVisible(false);
        }
    }

    Loader {
        id: loader
        sourceComponent: load

        anchors.centerIn: parent
    }

    Component{
        id: batt
        BatteryStatus{
            anchors.centerIn: parent

            width: cover.width - Theme.paddingLarge*2
            height: width

            percent: zik.battery
            timeleft: -1

            Text{
                visible: (zik.model == "ZIK1") || zik.demoMode

                anchors.bottom: parent.top
                anchors.margins: 0
                anchors.horizontalCenter: parent.horizontalCenter

                text: zik.demoMode ? "Demo mode" : "Legacy"
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
            }
        }
    }

    Component{
        id: load

        Text {
            text: qsTr("Not Connected")
            font.pixelSize: Theme.fontSizeMedium
            color: Theme.primaryColor
        }
    }

    CoverActionList {
        id: coverAction
        enabled: zik.isConnected()

        CoverAction {
            iconSource: "image://theme/icon-cover-next"

            onTriggered: {
                if(zik.capabilities & Zik.StreetMode ){
                    if(zik.ncType == "anc"){
                        zik.ncType = "aoc";
                    }else if(zik.ncType == "aoc"){
                        zik.ncType = "anc";
                    }
                }else if(zik.capabilities & Zik.Anc){
                    if(zik.anc == "true"){
                        zik.anc = "false";
                    }else{
                        zik.anc = "true";
                    }
                }
            }
        }
    }
}


