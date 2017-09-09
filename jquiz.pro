QT += qml quick quick-private multimedia

TEMPLATE = app
TARGET = jquiz
CONFIG += debug
INCLUDEPATH += .

HEADERS += quiz.h kanatextedit.h kana.h
SOURCES += main.cpp quiz.cpp kanatextedit.cpp kana.cpp
RESOURCES += jquiz.qrc
