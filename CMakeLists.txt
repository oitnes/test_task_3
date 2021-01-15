# project settings
cmake_minimum_required(VERSION 3.8)
project(test_task_3)

# environment setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# packet install
include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
set(CONAN_EXTERNAL_LIBS zlib/1.2.11 opencv/4.3.0@conan/stable boost/1.71.0@conan/stable)
conan_cmake_run(REQUIRES ${CONAN_EXTERNAL_LIBS} BASIC_SETUP BUILD missing)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_define_targets()

# include project sources
include_directories("./src")

# add project files
add_subdirectory("./src")

# add test files
option(BUILD_TESTS "build test runner executable" OFF)
if(BUILD_TESTS)
    add_subdirectory("./tests")
endif()
