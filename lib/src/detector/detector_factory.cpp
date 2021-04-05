#include "detector_factory.hpp"
#include "haar_detector.hpp"
#include "caffe_detector.hpp"
#include "error.hpp"


namespace {

#define GET_VALUE_CHECKED(settings, key, key_type, output_name)\
    if ((settings).find(std::string{(key)}) == (settings).not_found()) {                                                    \
        RAISE_ERROR(detection::CreationError, std::string{"type "} + std::string{(key)} + std::string{" is not exists"});   \
    }                                                                                                                       \
    key_type output_name;                                                                                                   \
    try {                                                                                                                   \
        output_name = (settings).get<key_type>(std::string{(key)});                                                         \
    }                                                                                                                       \
    catch (std::exception const &e) {                                                                                       \
        RAISE_ERROR(detection::CreationError, std::string{"incorrect \""} +                                                 \
        std::string{(key)} + std::string{"\" parameter type. need "} + std::string{#key_type});                             \
    }

#define GET_CHILD_CHECKED(settings, key, output_name)\
    if ((settings).find(std::string{(key)}) == (settings).not_found()) {                                                    \
        RAISE_ERROR(detection::CreationError, std::string{"type "} + std::string{(key)} + std::string{" is not exists"});   \
    }                                                                                                                       \
    boost::property_tree::ptree output_name;                                                                                \
    try {                                                                                                                   \
        output_name = (settings).get_child(std::string{(key)});                                                             \
    }                                                                                                                       \
    catch (std::exception const &e) {                                                                                       \
        RAISE_ERROR(detection::CreationError, std::string{"incorrect \""} + std::string{(key)} +                            \
        std::string{"\" parameter type. need object"});                                                                     \
    }

}

namespace detection {

    haar::Settings create_haar_cascade_detector_settings(const boost::property_tree::ptree &settings) {
        GET_VALUE_CHECKED(settings, "cascade_file_name", std::string, cascade_file_name);
        auto cascade_file_path = std::filesystem::current_path() / cascade_file_name;
        if (!std::filesystem::exists(cascade_file_path)) {
            RAISE_ERROR(CreationError, std::string("haar cascade detector xml file was not found by path: ") +
                                       cascade_file_path.c_str());
        }

        GET_VALUE_CHECKED(settings, "neighbors_number", int32_t, neighbors_number);
        GET_VALUE_CHECKED(settings, "scale_factor", float, scale_factor);

        GET_CHILD_CHECKED(settings, "max_object_size", max_object_size);
        GET_VALUE_CHECKED(max_object_size, "width", int32_t, max_object_size_width);
        GET_VALUE_CHECKED(max_object_size, "height", int32_t, max_object_size_height);

        GET_CHILD_CHECKED(settings, "min_object_size", min_object_size);
        GET_VALUE_CHECKED(min_object_size, "width", int32_t, min_object_size_width);
        GET_VALUE_CHECKED(min_object_size, "height", int32_t, min_object_size_height);

        return haar::Settings{scale_factor,
                              neighbors_number,
                              cascade_file_path,
                              cv::Size{min_object_size_width, min_object_size_height},
                              cv::Size{max_object_size_width, max_object_size_height}};
    }


    caffe::Settings create_caffe_cascade_detector_settings(const boost::property_tree::ptree &settings) {
        GET_VALUE_CHECKED(settings, "network_structure_file", std::string, network_structure_file_name);
        auto network_structure_file_path = std::filesystem::current_path() / network_structure_file_name;
        if (!std::filesystem::exists(network_structure_file_path)) {
            RAISE_ERROR(CreationError, std::string("network structure protobuf file was not found by path: ") +
                                       network_structure_file_path.c_str());
        }

        GET_VALUE_CHECKED(settings, "weights_file_name", std::string, weights_file_name);
        auto weights_file_path = std::filesystem::current_path() / weights_file_name;
        if (!std::filesystem::exists(weights_file_path)) {
            RAISE_ERROR(CreationError, std::string("network weights file was not found by path: ") +
                                       weights_file_path.c_str());
        }

        GET_VALUE_CHECKED(settings, "target_image_size", int, target_image_size);
        GET_VALUE_CHECKED(settings, "confidence_level", float, confidence_level);

        return caffe::Settings{network_structure_file_path, weights_file_path, target_image_size, confidence_level};
    }


    std::unique_ptr<Detector> create_detector(const boost::property_tree::ptree &settings) {
        GET_VALUE_CHECKED(settings, "type", std::string, detector_type_name);
        GET_CHILD_CHECKED(settings, "settings", detector_settings_object);

        if (detector_type_name == "haar") {
            return std::make_unique<detection::haar::HaarDetector>(
                    create_haar_cascade_detector_settings(detector_settings_object));
        } else if (detector_type_name == "caffe") {
            return std::make_unique<detection::caffe::CaffeDetector>(
                    create_caffe_cascade_detector_settings(detector_settings_object));
        }

        RAISE_ERROR(CreationError, detector_type_name + " is not implemented detector type");
    }

} // namespace detection