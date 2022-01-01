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
)

if (ENABLE_SPEECH_SYNTH)
    list(APPEND jquiz_SOURCES
        synth.cpp
        synth.h
        synththread.cpp
        synththread.h
    )
endif()

set(jquiz_RCS
    qml/qml.qrc
    assets/assets.qrc
)

if (WIN32)
    add_executable(jquiz WIN32 ${jquiz_SOURCES} ${jquiz_RCS})
else()
    add_executable(jquiz ${jquiz_SOURCES} ${jquiz_RCS})
endif()

target_compile_features(jquiz PRIVATE cxx_std_17)

if (ENABLE_SPEECH_SYNTH)
    target_compile_definitions(jquiz PRIVATE ENABLE_SPEECH_SYNTH)
endif()

target_link_libraries(jquiz
PRIVATE
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickPrivate
)

if (ENABLE_SPEECH_SYNTH)
    target_link_libraries(jquiz PRIVATE
        Qt5::Multimedia
        ThirdParty::htsengine
        ThirdParty::mecab
        ThirdParty::openjtalk
    )
endif()
