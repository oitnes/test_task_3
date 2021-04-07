#include "processor.h"

#include <boost/program_options.hpp>
#include <boost/dll/import.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <opencv2/imgcodecs.hpp>

#include <string>
#include <iostream>


namespace po = boost::program_options;
namespace dll = boost::dll;
namespace fs = boost::filesystem;


namespace config {
    constexpr int DEFAULT_WORKER_NUMBER = 2;
    constexpr const char *DEFAULT_DETECTOR_DESCRIPTION_FILE_NAME = "haar_detector_description.json";

#if defined(__linux__)
    constexpr const char *PLATFORM_LIB_NAME = "libdetection_processor_wrapper.so";
#elif defined(__APPLE__)
    constexpr const char *PLATFORM_LIB_NAME = "libdetection_processor_wrapper.dylib";
#elif defined(_WIN64)
    constexpr const char *PLATFORM_LIB_NAME = "detection_processor_wrapper.dll";
#elif defined(_WIN32)
    constexpr const char *PLATFORM_LIB_NAME = "detection_processor_wrapper.dll";
#else
    static_assert(false, "Unsupported platform");
#endif

} // namespace config


int main(int argc, const char **argv) {
    std::string detector_description_file;
    std::string images_dir;
    int workers_number;
    std::string library_path;

    po::options_description options_description("Computation options");
    options_description.add_options()
            ("help,h", "Show help")
            ("detector_description_file,d",
             po::value<std::string>(&detector_description_file)->default_value(
                     (fs::current_path() / config::DEFAULT_DETECTOR_DESCRIPTION_FILE_NAME).string()),
             "set up detector description file path")
            ("library_path,l",
             po::value<std::string>(&library_path)->default_value(
                     (fs::current_path() / config::PLATFORM_LIB_NAME).string()),
             "set processor library path")
            ("images_dir,i",
             po::value<std::string>(&images_dir)->required(),
             "set images folder path")
            ("workers_number,w",
             po::value<int>(&workers_number)->default_value(config::DEFAULT_WORKER_NUMBER),
             "set process worker number");

    po::variables_map vm;
    try {
        auto parsed = po::command_line_parser(argc, argv).options(options_description).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
    }
    catch (const po::error &error) {
        std::cerr << error.what();
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        options_description.print(std::cout);
        return EXIT_SUCCESS;
    }

    if (fs::exists(library_path)) {
        std::cout << std::string("Loading the processor library by path: ") + library_path + "\n";
    } else {
        std::cerr << std::string("The processor library was not found by path: ") + library_path + "\n";
        return EXIT_FAILURE;
    }

    boost::function<RESULT_CODE(int, const char *)> init_fn;
    boost::function<RESULT_CODE(const char *, NotificationFunction)> process_fn;
    try {
        init_fn = dll::import<RESULT_CODE(int, const char *)>(library_path, "init");
        process_fn = dll::import<RESULT_CODE(const char *, NotificationFunction)>(library_path, "process");
    } catch (const std::exception &error) {
        std::cerr << std::string("Library loading error: ") + error.what() + "\n";
        return EXIT_FAILURE;
    }

    auto init_result_code = init_fn(workers_number, detector_description_file.c_str());
    if (init_result_code != RESULT_CODE::INIT_SUCCESS) {
        std::cerr << "Library init failed\n";
        return EXIT_FAILURE;
    }

    auto callback = [](const char *result_json_str) {
        std::stringstream json_buffer_for_parse;
        json_buffer_for_parse << result_json_str;
        boost::property_tree::ptree result_json_root;
        boost::property_tree::read_json(json_buffer_for_parse, result_json_root);

        auto image_path_str = result_json_root.get_optional<std::string>("image_path");
        if (!image_path_str) {
            std::cerr << "can't get image path from result json.\n";
            return;
        }
        const std::string image_path{image_path_str.value()};

        auto img = cv::imread(image_path, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << std::string("can't open image by path: ") + image_path + "\n";
            return;
        }

        int detections_counter = 0;
        auto detections_json_array = result_json_root.get_child_optional("detections");
        if (detections_json_array) {
            BOOST_FOREACH(boost::property_tree::ptree::value_type &rowPair, detections_json_array.value()) {
                            detections_counter++;
                            auto current_image_path =
                                    image_path + ".face_" + std::to_string(detections_counter) + ".jpg";

                            auto &detection_obj = rowPair.second.get_child("");
                            auto x = detection_obj.get_optional<int>("x");
                            auto y = detection_obj.get_optional<int>("y");
                            auto width = detection_obj.get_optional<int>("width");
                            auto height = detection_obj.get_optional<int>("height");

                            cv::Mat flopped_face_roi;
                            cv::Mat face_roi(img, cv::Rect(x.value(), y.value(), width.value(), height.value()));
                            cv::flip(face_roi, flopped_face_roi, 0);
                            cv::imwrite(current_image_path, flopped_face_roi);

                            detection_obj.add("image_path", current_image_path);
                        }
        }

        std::cout << std::to_string(detections_counter) + std::string(" detections by path: ") + image_path + "\n";

        auto result_json_file_path = image_path + ".result.json";
        std::ofstream result_json_file(result_json_file_path);
        if (result_json_file) {
            boost::property_tree::write_json(result_json_file, result_json_root);
            result_json_file.close();
        } else {
            std::cerr << std::string("can't create result file by path: ") + result_json_file_path + "\n";
            return;
        }
    };
    auto process_result_code = process_fn(images_dir.c_str(), callback);
    if (process_result_code != RESULT_CODE::PROCESS_SUCCESS) {
        std::cerr << "Library image process failed\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
