#include "processor/processor.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <opencv2/imgcodecs.hpp>

#include <atomic>


BOOST_AUTO_TEST_CASE(processor_test_simple_by_haar_detector)
{
    const char *data = R"({
    "type": "haar",
    "settings": {
        "cascade_file_name": "haarcascade.xml",
        "neighbors_number": 3,
        "min_object_size": {
            "width": 10,
            "height": 10
        },
        "max_object_size": {
            "width": 200,
            "height": 200
        },
        "scale_factor": "2.0"
    }
})";

    std::filesystem::path detector_config_path(std::filesystem::current_path() / "config.json");
    std::ofstream file(detector_config_path);
    if (file) {
        file << data;
        file.close();
    } else {
        BOOST_CHECK(false);
    }

    processing::InitConfig init_config{4, detector_config_path.string()};

    processing::Processor processor;
    auto processor_init_result = processor.init(init_config);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(RESULT_CODE::INIT_SUCCESS),
                      static_cast<std::size_t>(processor_init_result));

    std::filesystem::path images_dir(std::filesystem::current_path() / "test_resources");
    std::atomic<std::size_t> images_counter = 0;
    std::atomic<std::size_t> faces_counter = 0;
    auto processor_process_result = processor.process(images_dir.string(),
                                                      [&images_counter, &faces_counter](
                                                              std::string processed_image_path,
                                                              std::vector<cv::Rect> faces) {
                                                          images_counter++;
                                                          faces_counter += faces.size();
                                                      });
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(RESULT_CODE::PROCESS_SUCCESS),
                      static_cast<std::size_t>(processor_process_result));
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(images_counter), 6);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(faces_counter), 3);

    std::filesystem::remove(detector_config_path);
}


BOOST_AUTO_TEST_CASE(processor_test_simple_by_caffe_detector)
{
    const char *data = R"({
    "type": "caffe",
    "settings": {
        "network_structure_file": "deploy.prototxt",
        "weights_file_name": "res10_300x300_ssd_iter_140000.caffemodel",
        "target_image_size": 300,
        "confidence_level": "0.97"
    }
})";

    std::filesystem::path detector_config_path(std::filesystem::current_path() / "config.json");
    std::ofstream file(detector_config_path);
    if (file) {
        file << data;
        file.close();
    } else {
        BOOST_CHECK(false);
    }

    processing::InitConfig init_config{4, detector_config_path.string()};

    processing::Processor processor;
    auto processor_init_result = processor.init(init_config);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(RESULT_CODE::INIT_SUCCESS),
                      static_cast<std::size_t>(processor_init_result));

    std::filesystem::path images_dir(std::filesystem::current_path() / "test_resources");
    std::atomic<std::size_t> images_counter = 0;
    std::atomic<std::size_t> faces_counter = 0;
    auto processor_process_result = processor.process(images_dir.string(),
                                                      [&images_counter, &faces_counter](
                                                              std::string processed_image_path,
                                                              std::vector<cv::Rect> faces) {
                                                          images_counter++;
                                                          faces_counter += faces.size();
                                                      });
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(RESULT_CODE::PROCESS_SUCCESS),
                      static_cast<std::size_t>(processor_process_result));
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(images_counter), 6);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(faces_counter), 3);

    std::filesystem::remove(detector_config_path);
}
