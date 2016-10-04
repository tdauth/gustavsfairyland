import QtQuick 2.0
import QtQuick.Controls 1.2
import QtMultimedia 5.6

Rectangle {
    width: 600
    height: 600
    color: "black"

    MediaPlayer {
        id: player
        objectName: "player"
        audioRole: MediaPlayer.GameRole
        source: "assets:/clips/mahler/bonus/trauer.mkv"
    }

    VideoOutput {
        id: videoOutput
        objectName: "videoOutput"
        source: player
        anchors.fill: parent
    }
}
