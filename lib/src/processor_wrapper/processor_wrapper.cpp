#include "processor/processor.hpp"

#include <boost/property_tree/json_parser.hpp>


std::unique_ptr<processing::Processor> ptr;

extern "C"
{

RESULT_CODE init(int workers_number, const char *detector_description_file_path) {
    if (ptr) {
        return RESULT_CODE::INIT_DOUBLE_INITIALIZATION;
    }

    ptr = std::make_unique<processing::Processor>();

    auto res = ptr->init(
            processing::InitConfig{static_cast<std::size_t>(workers_number), detector_description_file_path});
    return res;
}


RESULT_CODE process(const char *path_to_image_folder, NotificationFunction notification_fn_ptr) {
    if (!ptr) {
        return RESULT_CODE::PROCESS_UNINITIALIZED_LIB;
    }

    auto transformed_notification = [notification_fn_ptr](std::string processed_image_path,
                                                          std::vector<cv::Rect> faces) {

        boost::property_tree::ptree root;
        root.add("image_path", processed_image_path.c_str());
        boost::property_tree::ptree detections;
        for (auto &face: faces) {
            boost::property_tree::ptree detection_obj;
            detection_obj.add("x", face.x);
            detection_obj.add("y", face.y);
            detection_obj.add("width", face.width);
            detection_obj.add("height", face.height);
            detections.push_back(std::make_pair("", detection_obj));
        }
        root.add_child("detections", detections);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, root);
        try {
            (*notification_fn_ptr)(oss.str().c_str());
        } catch (...) {
            // pass
        }
    };

    auto res = ptr->process(std::string(path_to_image_folder), std::move(transformed_notification));
    return res;
}

}