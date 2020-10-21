#pragma once

#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <string>
#include <optional>


namespace detection {

    namespace config {
        static constexpr const char *FACE_CASCADE_FILE_NAME = "haarcascade_face.xml";
        static constexpr int NEIGHBORS_NUMBER = 3;
        static const cv::Size MIN_OBJECT_SIZE{10, 10};
        static const cv::Size MAX_OBJECT_SIZE{200, 200};
        static constexpr double THRESHOLD = 0.9;
    }

    struct DetectorSettings {
        double scale_factor;
        int neighbors_number;
        std::filesystem::path cascade_path;
        cv::Size minimum_face_size;
        cv::Size maximum_face_size;

        DetectorSettings() = delete;

        DetectorSettings(const DetectorSettings &) = default;

        DetectorSettings(DetectorSettings &&) = default;

        DetectorSettings &operator=(const DetectorSettings &) = default;

        DetectorSettings &operator=(DetectorSettings &&) = default;
    };

    class HaarDetector {
    public:
        explicit HaarDetector(DetectorSettings &&settings);

        std::vector<cv::Rect> detect(const cv::Mat &image);

    private:
        DetectorSettings _detector_settings;
        cv::CascadeClassifier _cascade_classifier;
    };

}