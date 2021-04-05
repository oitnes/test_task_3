#include "processor.hpp"

#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <string>


namespace processing {

    StatusCode Processor::init(const InitConfig &config) noexcept {

        if (!std::filesystem::exists(config.detector_description_file_path)) {
            return StatusCode::INIT_FILES_WAS_NOT_FOUND;
        }

        std::ifstream file(config.detector_description_file_path);
        std::stringstream buffer;
        if (file) {
            buffer << file.rdbuf();
            file.close();
        } else {
            return StatusCode::INIT_FILES_WAS_NOT_FOUND;
        }

        boost::property_tree::ptree detector_settings;
        try {
            boost::property_tree::read_json(buffer, detector_settings);
        }
        catch (std::exception const &e) {
            return StatusCode::INIT_BAD_SETTINGS_FILE;
        }


        try {
            _detector = detection::create_detector(detector_settings);
        } catch (const std::exception &error) {
            return StatusCode::INIT_BAD_DATA_FILE;
        } catch (...) {
            return StatusCode::INIT_UNEXPECTED_ERROR;
        }
        return StatusCode::INIT_SUCCESS;
    }


    StatusCode
    Processor::process(std::string path_to_image_folder, NotificationCallback &&notification) noexcept {
        std::set<std::string> extensions;
        extensions.emplace(".jpg");
        extensions.emplace(".bmp");
        extensions.emplace(".jpeg");

        if (!std::filesystem::exists(path_to_image_folder)) {
            return StatusCode::PROCESS_IMAGE_FOLDER_IS_NOT_EXISTS;
        }

        for (auto itEntry = std::filesystem::recursive_directory_iterator(path_to_image_folder);
             itEntry != std::filesystem::recursive_directory_iterator(); ++itEntry) {
            if (itEntry->is_regular_file()) {
                if (extensions.count(itEntry->path().filename().extension().string())) {
                    auto file_path = itEntry->path().string();
                    try {
                        auto img = cv::imread(file_path, cv::IMREAD_COLOR);
                        if (img.empty()) {
                            return StatusCode::PROCESS_BAD_IMAGE;
                        }
                        auto detections = _detector->detect(img);
                        notification(file_path, detections.size());
                    } catch (const cv::Exception &error) {
                        return StatusCode::PROCESS_BAD_IMAGE;
                    } catch (...) {
                        return StatusCode::PROCESS_UNEXPECTED_ERROR;
                    }
                }
            }
        }

        return StatusCode::PROCESS_SUCCESS;
    }

} // namespace processing