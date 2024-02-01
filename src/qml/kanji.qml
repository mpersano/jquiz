import QtQuick 2.3

Rectangle {
    id: card
    anchors.fill: parent
    focus: true
    state: "QUESTION"

    Answer {
        id: kanjiReading
        anchors.centerIn: parent
        width: parent.width
    }

    Text {
        id: eigo
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

    Keys.onPressed: function(event) {
        if (event.key == Qt.Key_Delete) {
            quiz.toggleCardMastered()
        } else if (event.key == Qt.Key_Space) {
            quiz.toggleCardReview()
        } else if (event.key == Qt.Key_Tab) {
            quiz.toggleReviewOnly();
        } else if (event.key == Qt.Key_P) {
            if (card.state == "ANSWER") {
                quiz.sayExample()
            }
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
