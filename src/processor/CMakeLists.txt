set(PROCESSOR_HEADERS processor_impl.hpp)
set(PROCESSOR_SOURCES processor_impl.cpp)

add_library(processor SHARED ${PROCESSOR_HEADERS} ${PROCESSOR_SOURCES})
target_include_directories(processor PRIVATE SYSTEM CONAN_PKG::boost)
target_include_directories(processor PUBLIC ./include)

target_link_libraries(processor detector_factory CONAN_PKG::boost CONAN_PKG::opencv CONAN_PKG::zlib)
