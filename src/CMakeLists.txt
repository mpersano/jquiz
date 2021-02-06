find_package(Qt5 COMPONENTS Qml Quick Multimedia REQUIRED)

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
    synth.cpp
    synth.h
    synththread.cpp
    synththread.h
)

set(jquiz_RCS
    qml/qml.qrc
    assets/assets.qrc
)

add_executable(jquiz ${jquiz_SOURCES} ${jquiz_RCS})

target_compile_features(jquiz PUBLIC cxx_std_17)

target_link_libraries(jquiz
PRIVATE
    ThirdParty::htsengine
    ThirdParty::mecab
    ThirdParty::openjtalk
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickPrivate
    Qt5::Multimedia
)
