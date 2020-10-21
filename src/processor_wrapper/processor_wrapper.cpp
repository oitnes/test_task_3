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

    void StatusWrapper::success(std::shared_ptr<StatusApi> &status) {
        status.reset(new StatusWrapper());
    }

    void StatusWrapper::images_folder_was_not_found(std::shared_ptr<StatusApi> &status, std::string_view message) {
        status.reset(new StatusWrapper(StatusCode::IMAGE_FOLDER_IN_NOT_EXISTS, message));
    }

    void StatusWrapper::bad_image(std::shared_ptr<StatusApi> &status, std::string_view message) {
        status.reset(new StatusWrapper(StatusCode::BAD_IMAGE, message));
    }

    void StatusWrapper::system_files_was_not_found(std::shared_ptr<StatusApi> &status, std::string_view message) {
        status.reset(new StatusWrapper(StatusCode::SYSTEM_FILES_WAS_NOT_FOUND, message));
    }

    void StatusWrapper::unexpected_error(std::shared_ptr<StatusApi> &status, std::string_view message) {
        status.reset(new StatusWrapper(StatusCode::UNEXPECTED, message));
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

    void ProcessorWrapper::init(std::shared_ptr<StatusApi> &status, InitConfig config) noexcept {
        const std::filesystem::path haar_cascades_folder{config.resource_folder_path};
        auto haar_path = haar_cascades_folder / detection::config::FACE_CASCADE_FILE_NAME;
        if (!std::filesystem::exists(haar_path)) {
            StatusWrapper::system_files_was_not_found(status,
                                                      std::string("resource file was not found at folder: ") +
                                                      haar_path.string());
            return;
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
            StatusWrapper::unexpected_error(status, error.what());
            return;
        } catch (...) {
            StatusWrapper::unexpected_error(status, "undefined error during detector initialization");
            return;
        }

        _extensions.emplace(".jpg");
        _extensions.emplace(".bmp");
        _extensions.emplace(".jpeg");

        StatusWrapper::success(status);
    }

    void ProcessorWrapper::process(std::shared_ptr<StatusApi> &status, std::string_view path_to_image_folder,
                                   NotificationCallback notification) noexcept {
        if (!std::filesystem::exists(path_to_image_folder)) {
            StatusWrapper::images_folder_was_not_found(status,
                                                       std::string("folder was not found: ") +
                                                       std::string(path_to_image_folder));
            return;
        }

        for (auto itEntry = std::filesystem::recursive_directory_iterator(path_to_image_folder);
             itEntry != std::filesystem::recursive_directory_iterator(); ++itEntry) {
            if (itEntry->is_regular_file()) {
                auto ext = itEntry->path().filename().extension().string();
                if (_extensions.count(ext)) {
                    auto file_path = itEntry->path().string();
                    try {
                        auto img = cv::imread(file_path, cv::IMREAD_COLOR);
                        auto detections = _detector->detect(img);
                        notification(file_path, detections.size());
                    } catch (const cv::Exception &error) {
                        StatusWrapper::bad_image(status, file_path);
                        return;
                    } catch (...) {
                        StatusWrapper::unexpected_error(status, std::string("at file: ") + file_path);
                        return;
                    }
                }
            }
        }

        StatusWrapper::success(status);
    }

} // namespace processor