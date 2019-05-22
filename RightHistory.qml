import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id:root
    property int currentWidth: root.width
    property int tabWidth: 28
    property int tabHeight: 60
    property bool isExpand: false
    Rectangle{
        id:historyList
        color: "#363636"
        height: parent.height
        anchors.right: parent.right
    }

    Rectangle{
        id:tab
        width: tabWidth
        height: tabHeight
        color: "#363636"
        anchors.right: historyList.left
        anchors.rightMargin: -5
        anchors.verticalCenter: parent.verticalCenter
        radius: 3
        opacity: 0.2
        Image{
            id:tabIcon
            anchors.fill: parent
            height: 20
            width: 20
            fillMode: Image.Stretch
            source: "qrc:/resources/left.svg"

        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled:true
            onEntered: {
                tab.opacity=0.8
            }
            onExited: {
                tab.opacity=0.2
            }
            onClicked: {
                isExpand=!isExpand
                console.log("tab:",isExpand)
                if(isExpand){
                   historyList.width=currentWidth
                    tabIcon.source="qrc:/resources/right.svg"
                }
                else
                {
                   historyList.width=0
                    tabIcon.source="qrc:/resources/left.svg"
                }
            }
        }
    }


}
