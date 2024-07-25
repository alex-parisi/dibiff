cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Dibiff VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(Dibiff test.cpp)

# Include directories for Eigen and miniaudio
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/Eigen)

# Include all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
target_sources(Dibiff PRIVATE ${SOURCES})

# Link pthread and m libraries
target_link_libraries(Dibiff pthread m)

# SIMD optimizations for GCC and Clang on x86_64 architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_options(Dibiff PRIVATE -msse2 -mavx2)
  endif()
endif()