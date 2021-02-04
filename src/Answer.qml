import QtQuick 2.3

Column {
    visible: false

    Text {
        id: kanji
        width: parent.width
        text: quiz.card.kanji
        font.pointSize: 60
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: reading
        width: parent.width
        text: "[" + quiz.card.readings[0] + "] "
        font.pointSize: 20
        horizontalAlignment: Text.AlignHCenter
    }

    Item { width: 1; height: 30 }

    Row {
        id: sample
        anchors.horizontalCenter: parent.horizontalCenter
        visible: quiz.example.isValid

        Column {
            width: 380
            Text {
                id: sampleNihongo
                width: parent.width
                text: quiz.example.jp
                wrapMode: Text.Wrap
                font.pointSize: 12
                horizontalAlignment: Text.AlignLeft
            }

            Text {
                id: sampleEigo
                width: parent.width
                text: quiz.example.en
                wrapMode: Text.Wrap
                font.pointSize: 12
                horizontalAlignment: Text.AlignLeft
            }
        }

        SpeechButton {
            width: 60
            height: 60
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
