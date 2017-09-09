import QtQuick 2.3
import JQuiz 1.0

Rectangle {
    id: card
    width: 480
    height: 320
    state: "QUESTION"

    Column {
        anchors.centerIn: parent
        width: parent.width
        visible: true

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

        Rectangle {
            width: card.width
            height: 50

            KanaTextEdit {
                id: reading
                font.pointSize: 30
                focus: true
                visible: true

                Keys.onPressed: {
                    if (event.key == Qt.Key_Return) {
                        reading.convertToKana(true);

                        if (reading.text == quiz.card.reading) {
                            reading.clear()
                            quiz.nextCard()
                        } else {
                            card.state = "ANSWER"
                            quiz.setCardReview();
                        }

                        event.accepted = true
                    }
                }
            }

            Text {
                id: answer
                width: card.width
                text: quiz.card.reading
                font.pointSize: 30
                horizontalAlignment: Text.AlignHCenter
                color: "red"
                focus: false
                visible: false

                Keys.onPressed: {
                    card.state = "QUESTION"
                    reading.clear()
                    quiz.nextCard()
                }
            }
        }
    }

    states: [
        State {
            name: "QUESTION"
            PropertyChanges { target: reading; visible: true }
            PropertyChanges { target: reading; focus: true }
            PropertyChanges { target: answer; visible: false }
            PropertyChanges { target: answer; focus: false }
        },
        State {
            name: "ANSWER"
            PropertyChanges { target: reading; visible: false }
            PropertyChanges { target: reading; focus: false }
            PropertyChanges { target: answer; visible: true }
            PropertyChanges { target: answer; focus: true }
        }
    ]
}
