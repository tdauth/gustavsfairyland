import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0

Rectangle {
    width: 800
    height: 600
    color: "black"

    MediaPlayer {
        id: player
        objectName: "player"
        source: "assets:/vampir/vampir.avi"
        //autoPlay: true
    }

    VideoOutput {
        id: videoOutput
        objectName: "videoOutput"
        source: player
        anchors.fill: parent
    }
}