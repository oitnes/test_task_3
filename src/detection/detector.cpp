#include "detector.hpp"
#include "config.hpp"
#include "utils.hpp"


namespace detection {

    HaarDetector::HaarDetector(DetectorSettings &&settings) : _detector_settings{std::move(settings)} {
        _cascade_classifier.load(_detector_settings.cascade_path.string());
    }


    std::vector<cv::Rect> HaarDetector::detect(const cv::Mat &image) {
        std::vector<cv::Rect> rects;
        _cascade_classifier.detectMultiScale(image, rects, _detector_settings.scale_factor,
                                             _detector_settings.neighbors_number,
                                             cv::CASCADE_SCALE_IMAGE, _detector_settings.minimum_face_size,
                                             _detector_settings.maximum_face_size);
        return std::move(rects);
    }


    FaceDetector::FaceDetector(const std::string &haar_cascade_path, double scale) :
            _scale{scale} {
        const std::filesystem::path haar_cascades_folder{haar_cascade_path};
        assert(std::filesystem::exists(haar_cascades_folder));

        DetectorSettings face_detector_settings{_scale,
                                                config::NEIGHBORS_NUMBER,
                                                haar_cascades_folder / config::FACE_CASCADE_FILE_NAME,
                                                config::MIN_OBJECT_SIZE,
                                                config::MAX_OBJECT_SIZE};
        _face_detector = std::make_unique<HaarDetector>(std::move(face_detector_settings));

        DetectorSettings eye_detector_settings{_scale,
                                               config::NEIGHBORS_NUMBER,
                                               haar_cascades_folder / config::EYE_CASCADE_FILE_NAME,
                                               config::MIN_OBJECT_SIZE,
                                               config::MAX_OBJECT_SIZE};
        _eye_detector = std::make_unique<HaarDetector>(std::move(eye_detector_settings));

        DetectorSettings smile_detector_settings{_scale,
                                                 config::NEIGHBORS_NUMBER,
                                                 haar_cascades_folder / config::SMILE_CASCADE_FILE_NAME,
                                                 config::MIN_OBJECT_SIZE,
                                                 config::MAX_OBJECT_SIZE};
        _smile_detector = std::make_unique<HaarDetector>(std::move(smile_detector_settings));

    }


    std::vector<FaceDetector::Detection> FaceDetector::detect(const cv::Mat &image) {
        auto prepared_image = utils::prepare(image, _scale);
        auto faces = _face_detector->detect(prepared_image);
        auto eyes = _eye_detector->detect(prepared_image);
        auto smiles = _smile_detector->detect(prepared_image);
        return std::move(sort(faces, eyes, smiles));
    }


    std::vector<FaceDetector::Detection> FaceDetector::sort(const std::vector<cv::Rect> &faces,
                                                            const std::vector<cv::Rect> &eyes,
                                                            const std::vector<cv::Rect> &smiles) const {
        std::vector<Detection> result;
        for(const auto& face: faces){

            for (const auto& eye: eyes){

            }

        }
        return std::move(result);
    }
}
