#include "detector_factory.hpp"
#include "haar_detector.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <exception>


namespace detection {

    std::unique_ptr<Detector> create_haar_cascade_detector(const boost::property_tree::ptree &settings) {
        const auto cascade_file_name = settings.get<std::string>("cascade_file_name");
        const auto neighbors_number = settings.get<int32_t>("neighbors_number");
        const auto scale_factor = settings.get<float>("scale_factor");
        const auto min_object_size_width = settings.get<int32_t>("min_object_size.width");
        const auto min_object_size_height = settings.get<int32_t>("min_object_size.height");
        const cv::Size min_object_size{min_object_size_width, min_object_size_height};
        const auto max_object_size_width = settings.get<int32_t>("max_object_size.width");
        const auto max_object_size_height = settings.get<int32_t>("max_object_size.height");
        const cv::Size max_object_size{max_object_size_width, max_object_size_height};

        auto cascade_file_path = std::filesystem::current_path() / cascade_file_name;
        if (!std::filesystem::exists(cascade_file_path)) {
            throw; // TODO
        }

        haar::Settings detector_settings{scale_factor,
                                         neighbors_number,
                                         cascade_file_path,
                                         min_object_size,
                                         max_object_size};
        std::unique_ptr<detection::Detector> detector = std::make_unique<detection::haar::HaarDetector>(
                std::move(detector_settings));

        return detector;
    }

    std::unique_ptr<Detector> create_detector(const std::filesystem::path &path_to_detector_description) {
        std::ifstream file(path_to_detector_description);
        std::stringstream buffer;

        if (file) {
            buffer << file.rdbuf();
            file.close();
        } else {
            throw; // TODO
        }

        try {
            boost::property_tree::ptree pt;
            boost::property_tree::read_json(buffer, pt);

            const auto detector_type_name = pt.get<std::string>("type");
            const auto settings_path = pt.get_child("settings");
            if (detector_type_name == "haar") {
                return create_haar_cascade_detector(settings_path);
            } else {
                throw; // TODO
            }
        }
        catch (std::exception const &e) {
            throw; // TODO
        }
    }

}