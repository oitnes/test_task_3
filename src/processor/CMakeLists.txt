set(PROCESSOR_HEADERS detector.hpp)
set(PROCESSOR_SOURCES detector.cpp)

add_library(processor STATIC ${PROCESSOR_HEADERS} ${PROCESSOR_SOURCES})
target_include_directories(processor PRIVATE SYSTEM CONAN_PKG::opencv)
target_link_libraries(processor CONAN_PKG::opencv CONAN_PKG::zlib)

set(RECOURSE_FILES "haarcascade_face.xml")

foreach (RESOURCE_FILE ${RECOURSE_FILES})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/recources/${RESOURCE_FILE}" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message("Build system copied file ${CMAKE_CURRENT_SOURCE_DIR}/recources/${RESOURCE_FILE} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endforeach ()

