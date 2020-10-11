set(PROCESSOR_WRAPPER_HEADERS processor_wrapper.hpp include/processor.hpp)
set(PROCESSOR_WRAPPER_SOURCES )

add_library(processor_wrapper SHARED ${PROCESSOR_WRAPPER_HEADERS} ${PROCESSOR_WRAPPER_SOURCES})
target_include_directories(processor_wrapper PRIVATE SYSTEM CONAN_PKG::opencv)
target_link_libraries(processor_wrapper CONAN_PKG::opencv CONAN_PKG::zlib)
