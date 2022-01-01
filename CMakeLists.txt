cmake_minimum_required(VERSION 3.7)

project(jquiz)

option(ENABLE_SPEECH_SYNTH "Enable speech synthesis" ON)

add_subdirectory(3rdparty)
add_subdirectory(src)
