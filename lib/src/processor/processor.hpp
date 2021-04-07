#pragma once

#include "processor_wrapper/include/processor.h"

#include "detector/detector_factory.hpp"

#include <functional>
#include <memory>
#include <thread>
#include <vector>


namespace processing {

    struct InitConfig {
        std::size_t workers_number;
        std::string detector_description_file_path;
    };


    using NotificationCallback = std::function<void(std::string processed_image_path,
                                                    std::vector<cv::Rect> faces)>;


    class Processor {
    public:
        ~Processor() = default;

        RESULT_CODE init(const InitConfig &config) noexcept;

        RESULT_CODE process(const std::string &path_to_image_folder, NotificationCallback &&notification) noexcept;

    private:
        const std::size_t _MAX_WORKER_COUNT{10};

        std::vector<std::unique_ptr<detection::Detector>> _detectors_pool;
    };

} // namespace processing