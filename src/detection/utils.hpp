#pragma once

#include <opencv2/imgproc.hpp>

namespace utils {

    cv::Mat prepare(const cv::Mat &image, double image_scale);

    bool inRect(const cv::Rect& border, const cv::Rect& inner, double threshold);
}