set(DETECTION_HEADERS detector.hpp config.hpp utils.hpp)
set(DETECTION_SOURCES detector.cpp utils.cpp)

add_library(detection STATIC ${DETECTION_HEADERS} ${DETECTION_SOURCES})
target_include_directories(detection PRIVATE SYSTEM CONAN_PKG::opencv)
target_link_libraries(detection CONAN_PKG::opencv CONAN_PKG::zlib)
