#include "processor.h"

#include <boost/program_options.hpp>
#include <boost/dll/import.hpp>

#include <string>
#include <iostream>


namespace po = boost::program_options;
namespace dll = boost::dll;
namespace fs = boost::filesystem;

namespace config {
    constexpr std::uint8_t DEFAULT_WORKER_NUMBER = 1;
    constexpr const char *DEFAULT_DETECTOR_DESCRIPTION_FILE_NAME = "haar_detector_description.json";
#ifdef __linux__
    constexpr const char *PLATFORM_LIB_NAME = "libprocessor.so";
#else
    // pass
#endif
} // namespace config


int main(int argc, const char **argv) {
    std::string detector_description_file;
    std::string images_dir;
    std::uint8_t workers_number;
    std::string library_path;

    po::options_description options_description("Evaluation options");
    options_description.add_options()
            ("help,h", "Show help")
            ("detector_description_file,d",
             po::value<std::string>(&detector_description_file)->default_value(
                     (fs::current_path() / config::DEFAULT_DETECTOR_DESCRIPTION_FILE_NAME).string()),
             "set detector description file path")
            ("library_path,l",
             po::value<std::string>(&library_path)->default_value(
                     (fs::current_path() / config::PLATFORM_LIB_NAME).string()),
             "set processor library path")
            ("images_dir,p",
             po::value<std::string>(&images_dir)->required(),
             "set images folder path")
            ("workers_number,n",
             po::value<std::uint8_t>(&workers_number)->default_value(config::DEFAULT_WORKER_NUMBER),
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
        std::cout << "Loading the processor library by path: " << library_path << std::endl;
    } else {
        std::cerr << "The processor library was not found by path: " << library_path;
        return EXIT_FAILURE;
    }

    boost::shared_ptr<ProcessorApi> plugin;
    try {
        plugin = dll::import<ProcessorApi>(
                fs::path{library_path},
                "Processor",
                dll::load_mode::append_decorations
        );
    } catch (const std::exception &error) {
        std::cerr << "Library loading error: " << error.what();
        return EXIT_FAILURE;
    }

    std::shared_ptr<StatusApi> status;

    plugin->init(status, InitConfig{workers_number, detector_description_file});

    if (!status->is_success()) {
        std::cerr << "Init failed: " << std::string(status->message());
        return EXIT_FAILURE;
    }

    plugin->process(status, images_dir, [](std::string_view processed_image_path, std::size_t faces_number) {
        std::cout << "detected faces: " << std::to_string(faces_number)
                  << ", by path: " << processed_image_path
                  << std::endl;
    });

    if (!status->is_success()) {
        std::cerr << "Processed failed[" << status->message() << "]";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
