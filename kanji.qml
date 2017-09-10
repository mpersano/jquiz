import QtQuick 2.3

Rectangle {
    id: card
    width: 480
    height: 320
    focus: true
    state: "QUESTION"

    Column {
        id: kanjiReading
        width: parent.width
        anchors.centerIn: parent
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
            text: quiz.card.reading
            font.pointSize: 30
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Text {
        id: eigo
        visible: true
        anchors.fill: parent
        text: quiz.card.eigo
        font.pointSize: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }

    states: [
        State {
            name: "QUESTION"
            PropertyChanges { target: kanjiReading; visible: false }
            PropertyChanges { target: eigo; visible: true }
        },
        State {
            name: "ANSWER"
            PropertyChanges { target: kanjiReading; visible: true }
            PropertyChanges { target: eigo; visible: false }
        }
    ]

    Keys.onPressed: {
        if (event.key == Qt.Key_X) {
            quiz.toggleCardMastered()
        } else if (event.key == Qt.Key_Space) {
            quiz.toggleCardReview()
        } else if (event.key == Qt.Key_Tab) {
            quiz.toggleReviewOnly();
        } else {
            if (card.state == "QUESTION") {
                card.state = "ANSWER";
            } else {
                card.state = "QUESTION"
                quiz.nextCard()
            }
        }
    }
}
