set(PROCESSOR_HEADERS
        "include/processor.h"
        )

set(PROCESSOR_SOURCES
        "processor_wrapper.cpp"
        )

add_library(detection_processor_wrapper SHARED ${PROCESSOR_HEADERS} ${PROCESSOR_SOURCES})
target_include_directories(detection_processor_wrapper PRIVATE SYSTEM CONAN_PKG::boost)
target_include_directories(detection_processor_wrapper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_link_libraries(detection_processor_wrapper detection_processor CONAN_PKG::boost CONAN_PKG::opencv CONAN_PKG::zlib)
