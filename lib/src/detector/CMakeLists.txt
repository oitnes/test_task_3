set(DETECTOR_HEADERS
        "error.hpp"
        "detector.hpp"
        "haar_detector.hpp"
        "caffe_detector.hpp"
        "detector_factory.hpp"
        )

set(DETECTOR_SOURCES
        "error.cpp"
        "haar_detector.cpp"
        "caffe_detector.cpp"
        "detector_factory.cpp"
        )

add_library(detector_factory STATIC ${DETECTOR_HEADERS} ${DETECTOR_SOURCES})
target_include_directories(detector_factory PRIVATE SYSTEM CONAN_PKG::opencv CONAN_PKG::boost)
target_link_libraries(detector_factory CONAN_PKG::opencv CONAN_PKG::zlib CONAN_PKG::boost)

set(RECOURSE_FILES
        "haarcascade.xml"
        "haar_detector_description.json"
        "deploy.prototxt"
        "res10_300x300_ssd_iter_140000.caffemodel"
        "caffe_detector_description.json"
        )

foreach (RESOURCE_FILE ${RECOURSE_FILES})
    set(CURRENT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/${RESOURCE_FILE}")
    file(COPY ${CURRENT_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message("Build system copied file ${CURRENT_FILE} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endforeach ()
