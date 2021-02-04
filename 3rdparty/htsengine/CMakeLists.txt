set(htsengine_SOURCES
    lib/HTS_audio.c
    lib/HTS_engine.c
    lib/HTS_gstream.c
    lib/HTS_hidden.h
    lib/HTS_label.c
    lib/HTS_misc.c
    lib/HTS_model.c
    lib/HTS_pstream.c
    lib/HTS_sstream.c
    lib/HTS_vocoder.c
)

set(htsengine_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(htsengine STATIC ${htsengine_SOURCES})

target_include_directories(htsengine
PUBLIC
    ${htsengine_INCLUDE_DIR}
)
