find_package(Qt6 COMPONENTS Qml Quick Multimedia REQUIRED)

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

set_target_properties(
    jquiz
    PROPERTIES CXX_STANDARD 20
               AUTOMOC ON
               AUTORCC ON
)

if (ENABLE_SPEECH_SYNTH)
    target_compile_definitions(jquiz PRIVATE ENABLE_SPEECH_SYNTH)
endif()

target_link_libraries(jquiz
PRIVATE
    Qt::Qml
    Qt::Quick
    Qt::QuickPrivate
)

if (ENABLE_SPEECH_SYNTH)
    target_link_libraries(jquiz PRIVATE
        Qt::Multimedia
        ThirdParty::htsengine
        ThirdParty::mecab
        ThirdParty::openjtalk
    )
endif()
