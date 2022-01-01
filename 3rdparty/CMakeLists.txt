if (ENABLE_SPEECH_SYNTH)
    add_subdirectory(htsengine)
    add_library(ThirdParty::htsengine ALIAS htsengine)

    add_subdirectory(mecab)
    add_library(ThirdParty::mecab ALIAS mecab)

    add_subdirectory(openjtalk)
    add_library(ThirdParty::openjtalk ALIAS openjtalk)
endif()
