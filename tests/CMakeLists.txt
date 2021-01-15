set(TEST_FILES
        main.cpp
        detector/detector.cpp)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests detector_factory CONAN_PKG::boost)

set(RECOURSE_FILES
        face_front_1_rgb.bmp
        haar_detector_description.json)

set(RESOURCE_TARGET_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_resources)
file(MAKE_DIRECTORY ${RESOURCE_TARGET_FOLDER})
foreach (RESOURCE_FILE ${RECOURSE_FILES})
    set(CURRENT_FILE "resources/${RESOURCE_FILE}")
    file(COPY ${CURRENT_FILE} DESTINATION ${RESOURCE_TARGET_FOLDER})
    message("Build system copied file ${CURRENT_FILE} to ${RESOURCE_TARGET_FOLDER}")
endforeach ()
