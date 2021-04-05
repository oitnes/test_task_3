set(TEST_FILES
        "main.cpp"
        "detector/haar_detector.cpp"
        "detector/caffe_detector.cpp"
        "processor/processor.cpp")

add_executable(test_runner ${TEST_FILES})
target_link_libraries(test_runner detector_factory detection_processor CONAN_PKG::boost)

target_include_directories(test_runner PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/")

set(RECOURSE_FILES
        "face_front_1_rgb.bmp"
        "cat_face_front_1_rgb.jpg"
        )

set(RESOURCE_TARGET_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_resources)
file(MAKE_DIRECTORY ${RESOURCE_TARGET_FOLDER})
foreach (RESOURCE_FILE ${RECOURSE_FILES})
    set(CURRENT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/${RESOURCE_FILE}")
    file(COPY ${CURRENT_FILE} DESTINATION ${RESOURCE_TARGET_FOLDER})
    message("Build system copied file ${CURRENT_FILE} to ${RESOURCE_TARGET_FOLDER}")
endforeach ()
