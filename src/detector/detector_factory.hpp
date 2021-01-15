#pragma once

#include "detector.hpp"

#include <memory>


namespace detection {

    std::unique_ptr<Detector> create_detector(const std::filesystem::path &path_to_detector_description);

}