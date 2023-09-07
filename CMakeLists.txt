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
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$$ORIGIN'")
    string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")
endif()

# ---- onnxruntime ---

# Look for onnxruntime files in <root>/lib
if(NOT DEFINED ONNXRUNTIME_ROOTDIR)
    if(NOT DEFINED ONNXRUNTIME_VERSION)
        set(ONNXRUNTIME_VERSION "1.14.1")
    endif()

    if(WIN32)
        # Windows x86-64
        set(ONNXRUNTIME_PREFIX "onnxruntime-win-x64-${ONNXRUNTIME_VERSION}")
        set(ONNXRUNTIME_EXT "zip")
    else()
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
            # Linux x86-64
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-x64-${ONNXRUNTIME_VERSION}")
        elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64)
            # Linux ARM 64-bit
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-aarch64-${ONNXRUNTIME_VERSION}")
        elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL armv7l)
            # Linux ARM 32-bit
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-arm32-${ONNXRUNTIME_VERSION}")
            set(ONNXRUNTIME_URL "https://github.com/synesthesiam/prebuilt-apps/releases/download/v1.0/onnxruntime-linux-arm32-${ONNXRUNTIME_VERSION}.tgz")
        else()
            message(FATAL_ERROR "Unsupported architecture for onnxruntime")
        endif()

        set(ONNXRUNTIME_EXT "tgz")
    endif()

    if(NOT DEFINED ONNXRUNTIME_URL)
        set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v${ONNXRUNTIME_VERSION}/${ONNXRUNTIME_PREFIX}.${ONNXRUNTIME_EXT}")
    endif()

    set(ONNXRUNTIME_FILENAME "${ONNXRUNTIME_PREFIX}.${ONNXRUNTIME_EXT}")
    set(ONNXRUNTIME_ROOTDIR "${CMAKE_CURRENT_LIST_DIR}/lib/${ONNXRUNTIME_PREFIX}")

    if(NOT EXISTS "${ONNXRUNTIME_ROOTDIR}")
        if(NOT EXISTS "download/${ONNXRUNTIME_FILENAME}")
            # Download onnxruntime release
            message("Downloading ${ONNXRUNTIME_URL}")
            file(DOWNLOAD "${ONNXRUNTIME_URL}" "download/${ONNXRUNTIME_FILENAME}")
        endif()

        # Extract .zip or .tgz to a directory like lib/onnxruntime-linux-x64-1.14.1/
        file(ARCHIVE_EXTRACT INPUT "download/${ONNXRUNTIME_FILENAME}" DESTINATION "${CMAKE_CURRENT_LIST_DIR}/lib")
    endif()
endif()

# ---- espeak-ng ---

if(DEFINED ESPEAK_NG_PKGCONFIG)
    # Locate espeak-ng with pkg-config
    find_package(PkgConfig)
    pkg_check_modules(ESPEAK_NG REQUIRED espeak-ng<2)
else()
    if(NOT DEFINED ESPEAK_NG_DIR)
        # Download from https://github.com/rhasspy/espeak-ng
        set(ESPEAK_NG_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/espeak-ng")
        if(NOT EXISTS "${ESPEAK_NG_DIR}")
            if(NOT DEFINED ESPEAK_NG_RELEASE)
                set(ESPEAK_NG_RELEASE "2023.9.7-4")
            endif()

            if(WIN32)
                # Windows x86-64
                set(ESPEAK_NG_FILENAME "windows_amd64.zip")
            else()
                if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
                    # Linux x86-64
                    set(ESPEAK_NG_FILENAME "linux_x64.tar.gz")
                elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64)
                    # Linux ARM 64-bit
                    set(ESPEAK_NG_FILENAME "linux_aarch64.tar.gz")
                elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL armv7l)
                    # Linux ARM 32-bit
                    set(ESPEAK_NG_FILENAME "linux_armv7.tar.gz")
                else()
                    message(FATAL_ERROR "Unsupported architecture for rhasspy espeak-ng")
                endif()
            endif()

            set(ESPEAK_NG_ARCHIVE "espeak-ng_${ESPEAK_NG_RELEASE}_${ESPEAK_NG_FILENAME}")

            if(NOT EXISTS "download/${ESPEAK_NG_ARCHIVE}")
                set(ESPEAK_NG_URL "https://github.com/rhasspy/espeak-ng/releases/download/${ESPEAK_NG_RELEASE}/${ESPEAK_NG_FILENAME}")
                message("Downloading ${ESPEAK_NG_URL}")
                file(DOWNLOAD "${ESPEAK_NG_URL}" "download/${ESPEAK_NG_ARCHIVE}")
            endif()

            # Extract .zip or .tar.gz to lib/espeak-ng
            file(ARCHIVE_EXTRACT INPUT "download/${ESPEAK_NG_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_LIST_DIR}/lib")
        endif()
    endif()

    set(ESPEAK_NG_INCLUDE_DIRS "${ESPEAK_NG_DIR}/include")
    set(ESPEAK_NG_LIBRARY_DIRS "${ESPEAK_NG_DIR}/lib")
    set(ESPEAK_NG_LIBRARIES "espeak-ng")
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

add_executable(piper_phonemize_exe src/main.cpp src/phoneme_ids.cpp)

if(NOT WIN32)
    set_property(TARGET piper_phonemize_exe PROPERTY OUTPUT_NAME piper_phonemize)
endif()

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
