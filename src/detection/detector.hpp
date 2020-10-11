#pragma once

#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <string>
#include <optional>


namespace detection {

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

    class FaceDetector {

        using FaceRect = cv::Rect;
        using EyeRect = cv::Rect;
        using Eyes = std::pair<std::optional<EyeRect>, std::optional<EyeRect>>;
        using SmileRect = cv::Rect;
    public:

        struct Detection {
            FaceRect face;
            std::optional<Eyes> eyes;
            std::optional<SmileRect> smile;
        };

        explicit FaceDetector(const std::string &haar_cascade_path, double scale = 1.);

        std::vector<Detection> detect(const cv::Mat &image);

    private:
        double _scale;

        std::unique_ptr<HaarDetector> _face_detector;
        std::unique_ptr<HaarDetector> _eye_detector;
        std::unique_ptr<HaarDetector> _smile_detector;

        std::vector<Detection> sort(const std::vector<cv::Rect> &faces,
                                    const std::vector<cv::Rect> &eyes,
                                    const std::vector<cv::Rect> &smiles) const;
    };

}