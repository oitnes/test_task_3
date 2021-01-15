#pragma once

#include "detector.hpp"

#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <mutex>


namespace detection {
    namespace haar {

        struct Settings {
            double scale_factor;
            int neighbors_number;
            std::filesystem::path cascade_path;
            cv::Size minimum_face_size;
            cv::Size maximum_face_size;

            Settings() = delete;

            Settings(const Settings &) = default;

            Settings(Settings &&) = default;

            Settings &operator=(const Settings &) = default;

            Settings &operator=(Settings &&) = default;
        };

        class HaarDetector : public Detector {
        public:
            explicit HaarDetector(Settings &&settings);

            ~HaarDetector() override = default;

            std::vector<cv::Rect> detect(const cv::Mat &image) override;

        private:
            std::mutex _mutex;
            Settings _detector_settings;
            cv::CascadeClassifier _cascade_classifier;
        };
    }
}