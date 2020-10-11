set(RECOURSE_FILES haarcascade_eye.xml haarcascade_face.xml haarcascade_smile.xml)

foreach(${RECOURSE_FILE} ${RECOURSE_FILES})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}\\${RECOURSE_FILE}" DESTINATION ${PROJECT_BINARY_DIR})
endforeach()