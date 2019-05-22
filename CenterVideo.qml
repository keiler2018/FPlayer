import QtQuick 2.0
import FVideo 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.3

Item {
    property string fileName: ""
    property string curTime: fvideo.currentTime
    property string totalTime: fvideo.totalTime

    //开始
    function start(){
        console.log("播放 按钮按下")
        if(fileName!="")
            fvideo.start(fileName)
    }

    //暂停
    function pause()
    {
        if(fileName!="")
            fvideo.pause()
    }

    //停止
    function stop()
    {
        console.log("停止 播放")
        fvideo.stop()
        stopForm.visible=true
        fileName=""
    }

    //调整声音
    function volume(value){
        fvideo.volume(value)
    }


    FileDialog {
          id: fileDialog
          title: "请选择一个视频文件"
          onAccepted: {

              fileName=fileDialog.fileUrl
              console.log("You chose: "+fileName)
              if(fileName!="")
              {
                 stopForm.visible=false
                 start()
              }
          }
          onRejected: {
              console.log("Canceled")
          }
          Component.onCompleted: visible = false
      }

    Rectangle{
        id:stopForm
        anchors.fill: parent
        color: "#000000"
        Rectangle{
            anchors.centerIn: parent
            width:140
            height: 40
            radius: 3
            color: "#212121"
            Row{
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5
                Image {
                    id: name
                    anchors.verticalCenter: parent.verticalCenter
                    height: 20
                    width: 20
                    fillMode: Image.Stretch
                    source: "qrc:/resources/fileOpen.svg"
                }
                Text {
                    id: text
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("打开文件")
                    font.pointSize:15
                    color: "white"
                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    fileDialog.open()
                }
            }
        }
    }
    FVideo{
        id:fvideo
        anchors.fill: parent
        z:stopForm.z-1
    }

}
