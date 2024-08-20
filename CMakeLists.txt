cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for Eigen, miniaudio, RtMidi, ImGui, and glad
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/Eigen)

# Include all source files from the 'src' directory and specific files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create an object library for shared sources
add_library(shared_sources OBJECT ${SOURCES})

# Add the executables and link the shared object library
add_executable(libTest ${PROJECT_SOURCE_DIR}/test/libTest.cpp)

# Link shared sources and other libraries
target_link_libraries(libTest PRIVATE $<TARGET_OBJECTS:shared_sources> ${CMAKE_DL_LIBS})

# Platform-specific settings and dependencies
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(libTest PRIVATE m)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_compile_definitions(shared_sources PRIVATE __MACOSX_CORE__)
  target_link_libraries(libTest PRIVATE m)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(shared_sources PRIVATE __WINDOWS_MM__)
  target_link_libraries(libTest PRIVATE winmm)
endif()

# SIMD optimizations for GCC and Clang on x86_64 architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_options(shared_sources PRIVATE -msse2 -mavx2)
  endif()
endif()

# # Add AddressSanitizer flags
# target_compile_options(shared_sources PRIVATE -fsanitize=address)
# target_link_options(flapTest PRIVATE -fsanitize=address)