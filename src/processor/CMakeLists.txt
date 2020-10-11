set(PROCESSOR_HEADERS image_queue.hpp)
set(PROCESSOR_SOURCES image_queue.cpp)

add_library(processor STATIC ${PROCESSOR_HEADERS} ${PROCESSOR_SOURCES})
target_include_directories(processor PRIVATE SYSTEM CONAN_PKG::opencv)
target_link_libraries(processor CONAN_PKG::opencv CONAN_PKG::zlib)
