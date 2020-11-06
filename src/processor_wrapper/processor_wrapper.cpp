#include "processor_wrapper.hpp"

#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <string>

namespace processor {

    StatusWrapper::StatusWrapper() :
            _status_code{StatusCode::SUCCESS}, _const_message{} {}

    StatusWrapper::StatusWrapper(StatusCode status_code, std::string_view message) :
            _status_code{status_code}, _const_message{message} {
    }

    std::unique_ptr<StatusApi> StatusWrapper::success() {
        return std::make_unique<StatusWrapper>();
    }

    std::unique_ptr<StatusApi> StatusWrapper::images_folder_was_not_found(std::string_view message) {
        return std::make_unique<StatusWrapper>(StatusCode::IMAGE_FOLDER_IN_NOT_EXISTS, message);
    }

    std::unique_ptr<StatusApi> StatusWrapper::bad_image(std::string_view message) {
        return std::make_unique<StatusWrapper>(StatusCode::BAD_IMAGE, message);
    }

    std::unique_ptr<StatusApi> StatusWrapper::system_files_was_not_found(std::string_view message) {
        return std::make_unique<StatusWrapper>(StatusCode::SYSTEM_FILES_WAS_NOT_FOUND, message);
    }

    std::unique_ptr<StatusApi> StatusWrapper::unexpected_error(std::string_view message) {
        return std::make_unique<StatusWrapper>(StatusCode::UNEXPECTED, message);
    }

    StatusWrapper::operator bool() const noexcept {
        return _status_code == StatusCode::SUCCESS;
    }

    bool StatusWrapper::is_success() const noexcept {
        return bool(*this);
    }

    StatusCode StatusWrapper::code() const noexcept {
        return _status_code;
    }

    const std::string_view &StatusWrapper::message() const noexcept {
        return _const_message;
    }

    std::unique_ptr<StatusApi> ProcessorWrapper::init(InitConfig config) noexcept {
        const std::filesystem::path haar_cascades_folder{config.resource_folder_path};
        auto haar_path = haar_cascades_folder / detection::config::FACE_CASCADE_FILE_NAME;
        if (!std::filesystem::exists(haar_path)) {
            return StatusWrapper::system_files_was_not_found(std::string("resource file was not found at folder: ") +
                                                             haar_path.string());
        }

        double scale_factor = 1.5;
        detection::DetectorSettings face_detector_settings{scale_factor,
                                                           detection::config::NEIGHBORS_NUMBER,
                                                           haar_path,
                                                           detection::config::MIN_OBJECT_SIZE,
                                                           detection::config::MAX_OBJECT_SIZE};
        try {
            _detector = std::make_unique<detection::HaarDetector>(std::move(face_detector_settings));
        } catch (const std::exception &error) {
            return StatusWrapper::unexpected_error(error.what());
        } catch (...) {
            return StatusWrapper::unexpected_error("undefined error during detector initialization");
        }

        _extensions.emplace(".jpg");
        _extensions.emplace(".bmp");
        _extensions.emplace(".jpeg");

        return StatusWrapper::success();
    }

    std::unique_ptr<StatusApi> ProcessorWrapper::process(std::string_view path_to_image_folder,
                                                         NotificationCallback notification) noexcept {
        if (!std::filesystem::exists(path_to_image_folder)) {
            return StatusWrapper::images_folder_was_not_found(std::string("folder was not found: ") +
                                                              std::string(path_to_image_folder));
        }
        // TODO: create multi thread process
        for (auto itEntry = std::filesystem::recursive_directory_iterator(path_to_image_folder);
             itEntry != std::filesystem::recursive_directory_iterator(); ++itEntry) {
            if (itEntry->is_regular_file()) {
                if (_extensions.count(itEntry->path().filename().extension().string())) {
                    auto file_path = itEntry->path().string();
                    try {
                        auto img = cv::imread(file_path, cv::IMREAD_COLOR);
                        auto detections = _detector->detect(img);
                        notification(file_path, detections.size());
                    } catch (const cv::Exception &error) {
                        return StatusWrapper::bad_image(file_path);
                    } catch (...) {
                        return StatusWrapper::unexpected_error(std::string("at file: ") + file_path);
                    }
                }
            }
        }

        return StatusWrapper::success();
    }

} // namespace processor