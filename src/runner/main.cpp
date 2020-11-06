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
#ifdef __linux__
    constexpr const char *PLATFORM_LIB_NAME = "libprocessor_wrapper.so";
#elif _WIN32
    constexpr const char *PLATFORM_LIB_NAME = "processor_wrapper.dll";
#else

#endif
} // namespace config


int main(int argc, const char **argv) {
    const std::uint8_t default_workers_number = config::DEFAULT_WORKER_NUMBER;
    const fs::path default_library_path = fs::current_path() / config::PLATFORM_LIB_NAME;

    std::string resource_dir;
    std::string images_dir;
    std::uint8_t workers_number;
    std::string library_path;

    po::options_description options_description("Evaluation options");
    options_description.add_options()
            ("help,h", "Show help")
            ("resource_dir,r",
             po::value<std::string>(&resource_dir)->default_value(fs::current_path().string()),
             "set processor recourse folder")
            ("library_path,l",
             po::value<std::string>(&library_path)->default_value(default_library_path.string()),
             "set processor recourse folder")
            ("images_dir,p", po::value<std::string>(&images_dir), "set images folder path")
            ("workers_number,n", po::value<std::uint8_t>(&workers_number)->default_value(default_workers_number),
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
        std::cout << "Loading the library by path: " << library_path << std::endl;
    } else {
        std::cerr << "The library not found by path: " << library_path;
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
        std::cerr << "Error during loading the library: " << error.what();
        return EXIT_FAILURE;
    }
    std::cout << "The library was loaded" << std::endl;

    auto init_status = plugin->init(InitConfig{workers_number, resource_dir});

    if (!init_status->is_success()) {
        std::cerr << "The library initialization was failed: " << std::string(init_status->message());
        return EXIT_FAILURE;
    }

    auto process_status = plugin->process(images_dir,
                                          [](std::string_view processed_image_path, std::size_t faces_number) {
                                              std::string message{"Detected faces: "};
                                              message += std::to_string(faces_number);
                                              message += std::string(", by path: ");
                                              message += processed_image_path;
                                              message += "\n";

                                              std::cout << message;
                                          });

    if (!init_status->is_success()) {
        std::cerr << "Folder process was failed[" << init_status->message() << "]";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
