cmake_minimum_required(VERSION 3.12)
project(HHOA_FSSP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)
include_directories(src)

# Find all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
)

# Remove main.cpp from library sources
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Create library
add_library(hhoa_fssp_lib ${SOURCES})

# Create executable
add_executable(hhoa_fssp src/main.cpp)
target_link_libraries(hhoa_fssp hhoa_fssp_lib)

# Create test executable (if tests exist)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    add_executable(hhoa_fssp_tests ${TEST_SOURCES})
    target_link_libraries(hhoa_fssp_tests hhoa_fssp_lib)
endif()

# Set output directories
set_target_properties(hhoa_fssp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(TARGET hhoa_fssp_tests)
    set_target_properties(hhoa_fssp_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
