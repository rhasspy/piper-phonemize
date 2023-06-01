cmake_minimum_required(VERSION 3.14)

project(
    piper_phonemize
    VERSION 0.0.1
    DESCRIPTION "Phonemization library for Piper text to speech system"
    HOMEPAGE_URL "https://github.com/rhasspy/piper-phonemize"
    LANGUAGES CXX
)

string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$ORIGIN'")
string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")

# ---- espeak-ng ---

find_package(PkgConfig)
pkg_check_modules(ESPEAK_NG REQUIRED espeak-ng<2)

# ---- Declare library ----

add_library(
    piper_phonemize SHARED
    src/phonemize.cpp
    src/phoneme_ids.cpp
)

target_include_directories(
    piper_phonemize PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    ${ESPEAK_NG_INCLUDE_DIRS}
)

target_link_directories(
    piper_phonemize PUBLIC
    ${ESPEAK_NG_LIBRARIES}
)

target_link_libraries(
    piper_phonemize
    ${ESPEAK_NG_LIBRARIES}
)

target_compile_options(
    piper_phonemize PUBLIC
    ${ESPEAK_NG_CFLAGS_OTHER}
)

target_compile_features(piper_phonemize PUBLIC cxx_std_17)
