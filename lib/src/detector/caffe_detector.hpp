#pragma once

#include "detector.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <mutex>


namespace detection {
    namespace caffe {

        struct Settings {
            std::filesystem::path net_structure_path;
            std::filesystem::path net_weights_path;
            int target_image_size;
            float confidence_level;

            Settings() = delete;

            Settings(const Settings &) = default;

            Settings(Settings &&) = default;

            Settings &operator=(const Settings &) = default;

            Settings &operator=(Settings &&) = default;
        };

        class CaffeDetector : public Detector {
        public:
            explicit CaffeDetector(Settings &&settings);

            ~CaffeDetector() override = default;

            std::vector<cv::Rect> detect(const cv::Mat &image) override;

        private:
            std::mutex _mutex;
            Settings _detector_settings;
            cv::dnn::Net _detector;

            cv::Mat prepare_image_for_detection(const cv::Mat &image) const;

            std::vector<cv::Rect>
            create_results(const cv::Mat &raw_results, int image_input_width, int image_input_height) const;
        };

    } // namespace haar
} // namespace detection