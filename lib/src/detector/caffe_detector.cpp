#include "caffe_detector.hpp"
#include "error.hpp"


namespace detection {
    namespace caffe {

        CaffeDetector::CaffeDetector(Settings &&settings) : _detector_settings{std::move(settings)} {
            // TODO: check all settings values for validity
            try {
                _detector = cv::dnn::readNetFromCaffe(_detector_settings.net_structure_path.string(),
                                                      _detector_settings.net_weights_path.string());
            }
            catch (std::exception const &e) {
                RAISE_ERROR(CreationError, std::string("loading of caffe model failed: ") + e.what());
            }
        }


        std::vector<cv::Rect> CaffeDetector::detect(const cv::Mat &image) {
            cv::Mat blob = cv::dnn::blobFromImage(prepare_image_for_detection(image));
            cv::Mat results;
            {
                std::lock_guard lk{_mutex};
                _detector.setInput(blob);
                results = _detector.forward();
            }

            return create_results(results, image.cols, image.rows);
        }


        cv::Mat CaffeDetector::prepare_image_for_detection(const cv::Mat &image) const {
            if (image.empty()) {
                RAISE_ERROR(ProcessingError, "empty image");
            }

            const auto input_image_max_dimension = cv::max(image.cols, image.rows);
            const float scale_factor = static_cast<float>(_detector_settings.target_image_size) /
                                       static_cast<float>(input_image_max_dimension);
            const auto resized_image_width = int(static_cast<float>(image.cols) * scale_factor);
            const auto resized_image_height = int(static_cast<float>(image.rows) * scale_factor);

            cv::Mat resized_image, prepared_image;
            cv::resize(image, resized_image, cv::Size(resized_image_width, resized_image_height));
            cv::copyMakeBorder(resized_image, prepared_image,
                               _detector_settings.target_image_size - resized_image_height,
                               0, 0,
                               _detector_settings.target_image_size - resized_image_width,
                               cv::BORDER_REPLICATE);

            return prepared_image;
        }


        std::vector<cv::Rect>
        CaffeDetector::create_results(const cv::Mat &raw_results, int image_input_width, int image_input_height) const {
            const int ARGUMENTS_NUMBER = 7; // model has 7 positional result arguments for every detection
            auto detections = raw_results.reshape(1, 1);
            const int detections_number = detections.cols / ARGUMENTS_NUMBER;

            std::vector<cv::Rect> result_rects;
            for (int current_detection = 0; current_detection < detections_number; current_detection++) {
                const int shift = current_detection * ARGUMENTS_NUMBER;
                // auto image_id = detections.at<float>(0, shift + 0);
                auto is_face = detections.at<float>(0, shift + 1);
                if ((0.97f > is_face) || (is_face > 1.03f)) { // face class equal 1
                    continue;
                }

                auto confidence = detections.at<float>(0, shift + 2);
                if (confidence < _detector_settings.confidence_level) {
                    continue;
                }

                auto left = static_cast<float>(image_input_width) * detections.at<float>(0, shift + 3);
                auto top = static_cast<float>(image_input_width) * detections.at<float>(0, shift + 4);
                auto right = static_cast<float>(image_input_height) * detections.at<float>(0, shift + 5);
                auto bottom = static_cast<float>(image_input_height) * detections.at<float>(0, shift + 6);
                auto height = int(bottom - top);
                auto width = int(right - left);

                result_rects.emplace_back(static_cast<int>(left),
                                          static_cast<int>(top),
                                          static_cast<int>(width),
                                          static_cast<int>(height));
            }

            return result_rects;
        }

    } // namespace caffe
} // namespace detection
