import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    id:root
    color: "#000000"
    opacity: 0.6
    height: 60
    property bool isStart: true
    property string curTime: "00:00"
    property string totalTime: "00:00"

    signal start()
    signal stop()
    signal pause()
    signal volume(var value)

    //开始 暂停
    Slider {
        id: curProgress
        anchors.left:parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.top: parent.top
        maximumValue: 100
        minimumValue: 0
        value: 0
        height: 15
        onValueChanged:{
        }
        style: SliderStyle {
            groove: Rectangle {
                width: parent.width
                height: 4
                color: "gray"
                Rectangle {
                    height: 4
                    width: styleData.handlePosition
                    color: "#70AB3A"
                }
            }
            handle: Rectangle {
                width: 15
                height: 15
                radius: 7.5
                color: "#70AB3A"
            }
        }
    }


    Image {
        id: startBtn
        height: 30
        width: 30
        anchors.centerIn: parent
        anchors.top: curProgress.bottom
        anchors.topMargin: 20
        source: "qrc:/resources/playhoverpressed.png"
        fillMode: Image.Stretch
        MouseArea{
            anchors.fill: parent
            onClicked: {
                    if(isStart){
                        root.start()
                        startBtn.source="qrc:/resources/pausehoverpressed.png";
                    }
                    else{
                        root.pause()
                        startBtn.source="qrc:/resources/playhoverpressed.png"
                    }
                    isStart=!isStart
                }
        }
    }

    Text {
        id: time
        text: curTime+" / "+totalTime
        color: "white"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    //停止
    Image {
        id: stop
        anchors.right: startBtn.left
        anchors.rightMargin: 10
        anchors.baseline: startBtn.baseline
        height: 30
        width: 30
        source: "qrc:/resources/stophoverpressed.png"
        fillMode: Image.Stretch
        MouseArea{
            anchors.fill: parent
            onClicked: {
                   root.stop()
                }
        }

    }


    //声音控制
    Image {
        id: audioImg
        anchors.left:startBtn.right
        anchors.leftMargin: 10
        anchors.baseline: startBtn.baseline
        height: 32
        width: 32
        source: "qrc:/resources/audio.svg"
    }
    Slider {
        id: audioValue
        anchors.left: audioImg.right
        anchors.verticalCenter: parent.verticalCenter

        width: 150
        maximumValue: 1
        minimumValue: 0
        value: 0.3
        onValueChanged:{
            if(value==0)
                audioImg.source="qrc:/resources/mute.svg"
            else
                audioImg.source="qrc:/resources/audio.svg"

            root.volume(value)
        }
        style: SliderStyle {
            groove: Rectangle {
                width: parent.width
                height: 4
                color: "gray"
                Rectangle {
                    height: 4
                    width: styleData.handlePosition
                    color: "#70AB3A"
                }
            }
            handle: Rectangle {
                width: 15
                height: 15
                radius: 7.5
                color: "#70AB3A"
            }
        }
    }
}
