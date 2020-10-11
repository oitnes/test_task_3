#pragma once

#include <opencv2/imgproc.hpp>

#include <filesystem>

namespace config {
    static constexpr const char *FACE_CASCADE_FILE_NAME = "haarcascade_face.xml";
    static constexpr const char *EYE_CASCADE_FILE_NAME = "haarcascade_eye.xml";
    static constexpr const char *SMILE_CASCADE_FILE_NAME = "haarcascade_smile.xml";
    static constexpr int NEIGHBORS_NUMBER = 3;
    static const cv::Size MIN_OBJECT_SIZE{10, 10};
    static const cv::Size MAX_OBJECT_SIZE{100, 100};
    static constexpr double THRESHOLD = 0.9;
}