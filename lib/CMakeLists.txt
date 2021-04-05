add_subdirectory(src)

option(BUILD_TESTS "build test runner executable" OFF)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()
