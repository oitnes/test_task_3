#include "detector.hpp"

namespace detection {

    HaarDetector::HaarDetector(DetectorSettings &&settings) : _detector_settings{std::move(settings)} {
        _cascade_classifier.load(_detector_settings.cascade_path.string());
    }

    std::vector<cv::Rect> HaarDetector::detect(const cv::Mat &image) {
        cv::Mat gray_image, small_gray_image;
        cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
        const double fx = 1 / _detector_settings.scale_factor;
        cv::resize(gray_image, small_gray_image, cv::Size(), fx, fx, cv::INTER_LINEAR);
        cv::equalizeHist(small_gray_image, small_gray_image);

        cv::Mat &prepared_image = small_gray_image;
        std::vector<cv::Rect> rects;
        _cascade_classifier.detectMultiScale(prepared_image, rects, _detector_settings.scale_factor,
                                             _detector_settings.neighbors_number,
                                             cv::CASCADE_SCALE_IMAGE, _detector_settings.minimum_face_size,
                                             _detector_settings.maximum_face_size);
        return std::move(rects);
    }

}
