#include "haar_detector.hpp"
#include "error.hpp"


namespace detection {
    namespace haar {

        HaarDetector::HaarDetector(Settings &&settings) : _detector_settings{std::move(settings)} {
            // TODO: check all settings values for validity
            try {
                _cascade_classifier.load(_detector_settings.cascade_path.string());
            }
            catch (std::exception const &e) {
                RAISE_ERROR(CreationError, std::string("loading of haar cascade failed ") + e.what());
            }
        }


        std::vector<cv::Rect> HaarDetector::detect(const cv::Mat &image) {
            cv::Mat prepared_image{prepare_image_for_detection(image)};

            std::vector<cv::Rect> rects;
            {
                std::lock_guard lk{_mutex};
                try {
                    _cascade_classifier.detectMultiScale(prepared_image, rects, _detector_settings.scale_factor,
                                                         _detector_settings.neighbors_number,
                                                         cv::CASCADE_SCALE_IMAGE, _detector_settings.minimum_face_size,
                                                         _detector_settings.maximum_face_size);
                }
                catch (std::exception const &e) {
                    RAISE_ERROR(CreationError, std::string("detection error: ") + e.what());
                }
            }

            return restore_rects(rects);
        }


        cv::Mat HaarDetector::prepare_image_for_detection(const cv::Mat &image) const {
            if (image.empty()) {
                RAISE_ERROR(ProcessingError, "empty image");
            }

            cv::Mat gray_image;
            if (image.channels() == 3) {
                cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
            } else if (image.channels() == 1) {
                gray_image = image.clone();
            } else {
                RAISE_ERROR(ProcessingError, "incorrect channels count");
            }

            cv::Mat small_gray_image;
            const double fx = 1 / _detector_settings.scale_factor;
            cv::resize(gray_image, small_gray_image, cv::Size(), fx, fx, cv::INTER_LINEAR);
            cv::equalizeHist(small_gray_image, small_gray_image);

            return small_gray_image;
        }


        std::vector<cv::Rect> HaarDetector::restore_rects(const std::vector<cv::Rect> &rects) const {
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

    } // namespace haar
} // namespace detection
