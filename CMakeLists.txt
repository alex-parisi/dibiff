cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Dibiff VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for Eigen, miniaudio, and RtMidi
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/Eigen)
include_directories(${PROJECT_SOURCE_DIR}/inc/rtmidi)

# Include all source files from the 'src' directory and specific files
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES ${PROJECT_SOURCE_DIR}/inc/rtmidi/RtMidi.cpp)

# Create an object library for shared sources
add_library(shared_sources OBJECT ${SOURCES})

# Add the executables and link the shared object library
add_executable(BabysFirstSynth ${PROJECT_SOURCE_DIR}/test/BabysFirstSynthTest.cpp)
target_link_libraries(BabysFirstSynth $<TARGET_OBJECTS:shared_sources>)

add_executable(ChorusTest ${PROJECT_SOURCE_DIR}/test/ChorusTest.cpp)
target_link_libraries(ChorusTest $<TARGET_OBJECTS:shared_sources>)

add_executable(SampleTest ${PROJECT_SOURCE_DIR}/test/SampleTest.cpp)
target_link_libraries(SampleTest $<TARGET_OBJECTS:shared_sources>)

# Platform-specific settings for RtMidi
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # ALSA support for Linux
    find_package(Threads REQUIRED)
    target_compile_definitions(shared_sources PRIVATE __LINUX_ALSA__)
    target_link_libraries(BabysFirstSynth asound Threads::Threads m)
    target_link_libraries(ChorusTest asound Threads::Threads m)
    target_link_libraries(SampleTest asound Threads::Threads m)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # CoreMIDI support for macOS
    target_compile_definitions(shared_sources PRIVATE __MACOSX_CORE__)
    find_library(COREMIDI CoreMIDI)
    find_library(COREAUDIO CoreAudio)
    find_library(COREFOUNDATION CoreFoundation)
    target_link_libraries(BabysFirstSynth ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION} m)
    target_link_libraries(ChorusTest ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION} m)
    target_link_libraries(SampleTest ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION} m)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Multimedia Library support for Windows
    target_compile_definitions(shared_sources PRIVATE __WINDOWS_MM__)
    target_link_libraries(BabysFirstSynth winmm)
    target_link_libraries(ChorusTest winmm)
    target_link_libraries(SampleTest winmm)
endif()

# SIMD optimizations for GCC and Clang on x86_64 architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_options(shared_sources PRIVATE -msse2 -mavx2)
  endif()
endif()