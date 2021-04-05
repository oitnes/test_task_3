#pragma once

#include <opencv2/imgproc.hpp>

#include <vector>


namespace detection {

    class Detector {
    public:
        virtual ~Detector() = default;

        virtual std::vector<cv::Rect> detect(const cv::Mat &image) = 0;
    };

} // namespace detection