#include "utils.hpp"


namespace utils {

    cv::Mat prepare(const cv::Mat &image, double image_scale) {
        cv::Mat gray_image, small_gray_image;
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
        const double fx = 1 / image_scale;
        cv::resize(gray_image, small_gray_image, cv::Size(), fx, fx, cv::INTER_LINEAR);
        cv::equalizeHist(small_gray_image, small_gray_image);
        return std::move(small_gray_image);
    }


    bool inRect(const cv::Rect& border, const cv::Rect& inner, double threshold){
        return false;
    }

}
