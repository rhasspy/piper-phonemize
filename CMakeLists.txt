cmake_minimum_required(VERSION 3.14)

set(CMAKE_VERBOSE_MAKEFILE off)

project(
    piper_phonemize
    VERSION 0.0.1
    DESCRIPTION "Phonemization library for Piper text to speech system"
    HOMEPAGE_URL "https://github.com/rhasspy/piper-phonemize"
    LANGUAGES CXX
)

string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$ORIGIN'")
string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")

# lib/Linux-x86_64
# lib/Linux-aarch64
set(ONNXRUNTIME_ROOTDIR ${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/onnxruntime)

# ---- espeak-ng ---

find_package(PkgConfig)
pkg_check_modules(ESPEAK_NG REQUIRED espeak-ng<2)

# ---- Declare library ----

add_library(
    piper_phonemize SHARED
    src/phonemize.cpp
    src/phoneme_ids.cpp
    src/tashkeel.cpp
)

target_include_directories(
    piper_phonemize PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    ${ESPEAK_NG_INCLUDE_DIRS}
    ${ONNXRUNTIME_ROOTDIR}/include
)

target_link_directories(
    piper_phonemize PUBLIC
    ${ESPEAK_NG_LIBRARY_DIRS}
    ${ONNXRUNTIME_ROOTDIR}/lib
)

target_link_libraries(
    piper_phonemize
    ${ESPEAK_NG_LIBRARIES}
    onnxruntime
)

target_compile_options(
    piper_phonemize PUBLIC
    ${ESPEAK_NG_CFLAGS_OTHER}
)

target_compile_features(piper_phonemize PUBLIC cxx_std_17)

# ---- Declare executable ----

add_executable(piper_phonemize_exe src/main.cpp)
set_property(TARGET piper_phonemize_exe PROPERTY OUTPUT_NAME piper_phonemize)

target_compile_features(piper_phonemize_exe PUBLIC cxx_std_17)

target_include_directories(
    piper_phonemize_exe PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    ${ESPEAK_NG_INCLUDE_DIRS}
)

target_link_directories(
    piper_phonemize_exe PUBLIC
    ${ESPEAK_NG_LIBRARY_DIRS}
)

target_link_libraries(piper_phonemize_exe PUBLIC
                      piper_phonemize
                      ${ESPEAK_NG_LIBRARIES})

target_compile_options(
    piper_phonemize_exe PUBLIC
    ${ESPEAK_NG_CFLAGS_OTHER}
)

# ---- Declare install targets ----

install(
    TARGETS piper_phonemize
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include/piper-phonemize
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp")

install(
    TARGETS piper_phonemize_exe
    ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR})
