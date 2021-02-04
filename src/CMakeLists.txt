find_package(Qt5 COMPONENTS Qml Quick REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(jquiz_SOURCES
    kana.cpp
    kana.h
    kanatextedit.cpp
    kanatextedit.h
    main.cpp
    quiz.cpp
    quiz.h
)

set(jquiz_RCS
    jquiz.qrc
)

add_executable(jquiz ${jquiz_SOURCES} ${jquiz_RCS})

target_link_libraries(jquiz
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickPrivate
)

