add_executable(runner main.cpp)
target_include_directories(runner PRIVATE SYSTEM CONAN_PKG::boost)
target_link_libraries(runner detection  CONAN_PKG::opencv CONAN_PKG::zlib CONAN_PKG::boost)
