import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    visible: true
    width: 800
    height: 480
    title: qsTr("FPlayer")


    CenterVideo{
        id:centerVideo
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: history.right
        z:history.z-2
    }


    RightHistory{
        id:history
        width: 180
        currentWidth:history.width
        anchors.right: parent.right
        anchors.top:parent.top
        anchors.bottom: parent.bottom
    }

    BottomTool{
        id:tool
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right:parent.right
        z:history.z-1
        curTime: centerVideo.curTime
        totalTime: centerVideo.totalTime
        onStart: {
            centerVideo.start()
        }
        onPause: {
            centerVideo.pause()
        }

        onStop: {
            centerVideo.stop()
        }
        onVolume: {
            centerVideo.volume(value)
        }
    }

}
