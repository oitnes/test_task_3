set(PROCESSOR_WRAPPER_HEADERS processor_wrapper.hpp)
set(PROCESSOR_WRAPPER_SOURCES processor_wrapper.cpp)

add_library(processor_wrapper SHARED ${PROCESSOR_WRAPPER_HEADERS} ${PROCESSOR_WRAPPER_SOURCES})
target_include_directories(processor_wrapper PRIVATE SYSTEM CONAN_PKG::boost)
target_include_directories(processor_wrapper PRIVATE ./include)
target_link_libraries(processor_wrapper processor CONAN_PKG::boost CONAN_PKG::opencv CONAN_PKG::zlib)
