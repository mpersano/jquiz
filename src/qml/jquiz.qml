import QtQuick 2.3

Rectangle {
    width: 480
    height: 320
    color: "white"

    Loader {
        source: cardSource
        focus: true
        anchors.fill: parent
    }

    Text {
        id: reviewFlag
        visible: quiz.card.deck == 'R'
        anchors.top: parent.top
        anchors.left: parent.left
        text: "(,,ﾟДﾟ)"
        font.pointSize: 20
        color: "red"
    }

    Text {
        id: masteredFlag
        visible: quiz.card.deck == 'M'
        anchors.top: parent.top
        anchors.left: parent.left
        text: " (≧∇≦)ｷｬｰ♪  "
        font.pointSize: 20
        color: "green"
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        text: quiz.statusLine
        font.pointSize: 20
    }
}
