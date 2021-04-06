#pragma once

#include "detector/detector_factory.hpp"

#include <functional>
#include <memory>
#include <set>
#include <thread>
#include <vector>

#include <cstdint>


enum class StatusCode : std::size_t {
    SUCCESS = 0,
    UNEXPECTED_ERROR = SUCCESS + 1,

    INIT_SUCCESS = 100,
    INIT_UNEXPECTED_ERROR = INIT_SUCCESS + 1,
    INIT_FILES_WAS_NOT_FOUND = INIT_SUCCESS + 2,
    INIT_BAD_SETTINGS_FILE = INIT_SUCCESS + 3,
    INIT_BAD_DATA_FILE = INIT_SUCCESS + 4,
    INIT_INCORRECT_WORKER_NUMBER = INIT_SUCCESS + 5,

    PROCESS_SUCCESS = 200,
    PROCESS_UNEXPECTED_ERROR = PROCESS_SUCCESS + 1,
    PROCESS_IMAGE_FOLDER_IS_NOT_EXISTS = PROCESS_SUCCESS + 2,
    PROCESS_BAD_IMAGE = PROCESS_SUCCESS + 3,

};

struct InitConfig {
    std::size_t workers_number;
    std::string detector_description_file_path;
};

using NotificationCallback = std::function<void(std::string processed_image_path,
                                                std::size_t faces_number)>;

namespace processing {

    class Processor {
    public:
        ~Processor() = default;

        StatusCode init(const InitConfig &config) noexcept;

        StatusCode process(const std::string &path_to_image_folder, NotificationCallback &&notification) noexcept;

    private:
        std::size_t _MAX_WORKER_COUNT = 10;

        std::vector<std::unique_ptr<detection::Detector>> _detectors_pool;
    };

} // namespace processing