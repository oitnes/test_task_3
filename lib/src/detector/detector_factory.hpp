#pragma once

#include "haar_detector.hpp"

#include <boost/property_tree/ptree.hpp>

#include <memory>


namespace detection {

    std::unique_ptr<Detector> create_detector(const boost::property_tree::ptree &settings);

} // namespace detection
