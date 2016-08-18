import QtQuick 2.0
import QtQuick.Controls 1.2
import QtMultimedia 5.6

Rectangle {
    width: 200
    height: 200
    color: "black"

    MediaPlayer {
        id: player
        objectName: "player"
        audioRole: GameRole
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
