#include "haar_detector.hpp"

#include <boost/exception/all.hpp>


namespace detection {
    namespace haar {

        typedef boost::error_info<struct tag_errmsg, std::string> errmsg_info;

        struct allocation_failed : public boost::exception, public std::exception
        {
            const char *what() const noexcept { return "allocation failed"; }
        };


        HaarDetector::HaarDetector(Settings &&settings) : _detector_settings{std::move(settings)} {
            try {
                _cascade_classifier.load(_detector_settings.cascade_path.string());
            }
            catch (std::exception const &e) {
                throw; // TODO
            }
        }

        std::vector<cv::Rect> HaarDetector::detect(const cv::Mat &image) {
            if(image.empty()){
                throw; // TODO
            }
            // prepare image to process
            cv::Mat gray_image, small_gray_image;
            cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
            const double fx = 1 / _detector_settings.scale_factor;
            cv::resize(gray_image, small_gray_image, cv::Size(), fx, fx, cv::INTER_LINEAR);
            cv::equalizeHist(small_gray_image, small_gray_image);

            // detect
            std::lock_guard lk{_mutex};
            cv::Mat &prepared_image = small_gray_image;
            std::vector<cv::Rect> rects;
            _cascade_classifier.detectMultiScale(prepared_image, rects, _detector_settings.scale_factor,
                                                 _detector_settings.neighbors_number,
                                                 cv::CASCADE_SCALE_IMAGE, _detector_settings.minimum_face_size,
                                                 _detector_settings.maximum_face_size);

            // restore normal coordinates
            std::vector<cv::Rect> result_rects;
            for (const auto &rect: rects) {
                auto x = rect.x * _detector_settings.scale_factor;
                auto y = rect.y * _detector_settings.scale_factor;
                auto width = rect.width * _detector_settings.scale_factor;
                auto height = rect.height * _detector_settings.scale_factor;
                result_rects.emplace_back(static_cast<int>(x),
                                          static_cast<int>(y),
                                          static_cast<int>(width),
                                          static_cast<int>(height));
            }

            return result_rects;
        }
    }
}
