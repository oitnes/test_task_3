set(TEST_FILES
        "main.cpp"
        "detector/haar_detector.cpp"
        "detector/caffe_detector.cpp"
        "processor/processor.cpp")

add_executable(test_runner ${TEST_FILES})
target_link_libraries(test_runner detector_factory detection_processor CONAN_PKG::boost)
target_include_directories(test_runner PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test_images/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_resources/")
