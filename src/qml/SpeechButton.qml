import QtQuick 2.2
import QtQuick.Layouts 1.1
import JQuiz 1.0

Item {
    id: root
    state: curSynthState()

    function curSynthState()
    {
        if (quiz.synthState == Quiz.Idle)
            return "idle"
        if (quiz.synthState == Quiz.Loading)
            return "loading"
        return "playing"
    }

    Image {
        id: playButton
        source: "play.png"
        anchors.centerIn: parent

        MouseArea {
            anchors.fill: parent
            onClicked: quiz.sayExample()
        }
    }

    Image {
        id: loadingSpinner
        source: "spinner.png"
        anchors.centerIn: parent

        NumberAnimation on rotation {
            from: 0
            to: 360
            running: loadingSpinner.visible == true
            loops: Animation.Infinite
            duration: 1600
        }
    }

    Image {
        id: stopButton
        source: "stop.png"
        anchors.centerIn: parent

        MouseArea {
            anchors.fill: parent
            onClicked: quiz.stopSynth()
        }
    }

    states: [
        State {
            name: "idle"
            PropertyChanges { target: playButton; visible: true }
            PropertyChanges { target: loadingSpinner; visible: false }
            PropertyChanges { target: stopButton; visible: false }
        },
        State {
            name: "loading"
            PropertyChanges { target: playButton; visible: false }
            PropertyChanges { target: loadingSpinner; visible: true }
            PropertyChanges { target: stopButton; visible: false }
        },
        State {
            name: "playing"
            PropertyChanges { target: playButton; visible: false }
            PropertyChanges { target: loadingSpinner; visible: false }
            PropertyChanges { target: stopButton; visible: true }
        }
    ]
}
