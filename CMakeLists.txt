cmake_minimum_required(VERSION 3.14)

set(CMAKE_VERBOSE_MAKEFILE off)

project(
    piper_phonemize
    VERSION 1.2.0
    DESCRIPTION "Phonemization library for Piper text to speech system"
    HOMEPAGE_URL "https://github.com/rhasspy/piper-phonemize"
    LANGUAGES CXX
)

if(WIN32)

# Force compiler to use UTF-8 for IPA constants
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

else()

# Linux flags
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$$ORIGIN' -I/usr/include/espeak-ng")
string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")

endif()

# lib/Linux-x86_64
# lib/Linux-aarch64
# lib/Windows-AMD64
if(NOT DEFINED ONNXRUNTIME_ROOTDIR)

# Look for onnxruntime files in <root>/lib
set(ONNXRUNTIME_ROOTDIR ${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/onnxruntime)

endif()

# ---- espeak-ng ---

if(DEFINED ESPEAK_NG_DIR)

# Expect -DESPEAK_NG_DIR on Windows
set(ESPEAK_NG_INCLUDE_DIRS "${ESPEAK_NG_DIR}/include")
set(ESPEAK_NG_LIBRARY_DIRS "${ESPEAK_NG_DIR}/lib")
set(ESPEAK_NG_LIBRARIES "espeak-ng")

else()

# Locate espeak-ng with pkg-config
find_package(PkgConfig)
pkg_check_modules(ESPEAK_NG REQUIRED espeak-ng<2)

endif()

# ---- Declare library ----

add_library(
    piper_phonemize SHARED
    src/phonemize.cpp
    src/phoneme_ids.cpp
    src/tashkeel.cpp
    src/shared.cpp
)

set_target_properties(piper_phonemize PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

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
