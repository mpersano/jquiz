import QtQuick 2.3

Rectangle {
    width: 480
    height: 320
    focus: true
    color: "white"

    Rectangle {
        id: card
        anchors.fill: parent
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

    Keys.onPressed: {
        if (event.key == Qt.Key_X) {
            quiz.toggleCardMastered()
        } else if (event.key == Qt.Key_M) {
            quiz.toggleCardReview()
        } else if (event.key == Qt.Key_P) {
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
