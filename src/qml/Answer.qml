import QtQuick 2.3
import JQuiz 1.0

Column {
    visible: false

    Text {
        id: eigo
        width: parent.width
        text: quiz.card.eigo
        font.pointSize: 20
        horizontalAlignment: Text.AlignHCenter
    }

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
        text: quiz.card.readings[0]
        font.pointSize: 20
        horizontalAlignment: Text.AlignHCenter
    }

    Item { width: 1; height: 30 }

    Row {
        id: sample
        anchors.horizontalCenter: parent.horizontalCenter
        visible: quiz.example.isValid

        Column {
            width: 500
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
            width: 40
            height: 40
            anchors.verticalCenter: parent.verticalCenter
            visible: quiz.synthState != Quiz.Error
        }
    }
}
