cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Dibiff VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(Dibiff test.cpp)

# Include directories for Eigen, miniaudio, and RtMidi
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/Eigen)
include_directories(${PROJECT_SOURCE_DIR}/inc/rtmidi)

# Include all source files from the 'src' directory and specific files
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES ${PROJECT_SOURCE_DIR}/inc/rtmidi/RtMidi.cpp)
target_sources(Dibiff PRIVATE ${SOURCES})

# Platform-specific settings for RtMidi
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # ALSA support for Linux
    find_package(Threads REQUIRED)
    target_compile_definitions(Dibiff PRIVATE __LINUX_ALSA__)
    target_link_libraries(Dibiff asound Threads::Threads)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # CoreMIDI support for macOS
    target_compile_definitions(Dibiff PRIVATE __MACOSX_CORE__)
    find_library(COREMIDI CoreMIDI)
    find_library(COREAUDIO CoreAudio)
    find_library(COREFOUNDATION CoreFoundation)
    target_link_libraries(Dibiff ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Multimedia Library support for Windows
    target_compile_definitions(Dibiff PRIVATE __WINDOWS_MM__)
    target_link_libraries(Dibiff winmm)
endif()

# # Optional JACK support for Linux and macOS
# if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#     find_package(PkgConfig REQUIRED)
#     pkg_check_modules(JACK jack)
#     if(JACK_FOUND)
#         target_compile_definitions(Dibiff PRIVATE __UNIX_JACK__)
#         target_include_directories(Dibiff PRIVATE ${JACK_INCLUDE_DIRS})
#         target_link_libraries(Dibiff ${JACK_LIBRARIES})
#     endif()
# endif()

# SIMD optimizations for GCC and Clang on x86_64 architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_options(Dibiff PRIVATE -msse2 -mavx2)
  endif()
endif()