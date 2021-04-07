set(PROCESSOR_HEADERS
        "processor.hpp"
        )

set(PROCESSOR_SOURCES
        "processor.cpp"
        )

add_library(detection_processor STATIC ${PROCESSOR_HEADERS} ${PROCESSOR_SOURCES})
target_include_directories(detection_processor PRIVATE SYSTEM CONAN_PKG::boost)
target_include_directories(detection_processor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_link_libraries(detection_processor detector_factory CONAN_PKG::boost CONAN_PKG::opencv CONAN_PKG::zlib)
