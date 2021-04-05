#include "detector/detector_factory.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <opencv2/imgcodecs.hpp>


BOOST_AUTO_TEST_CASE(haar_detector_test_with_face)
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
    std::stringstream buffer;
    buffer << data;

    boost::property_tree::ptree detector_settings;
    boost::property_tree::read_json(buffer, detector_settings);

    auto detector = detection::create_detector(detector_settings);

    auto loaded_image = cv::imread(
            (std::filesystem::current_path() / "test_resources" / "face_front_1_rgb.bmp").string(),
            cv::IMREAD_COLOR);

    auto detections = detector->detect(loaded_image);

    BOOST_CHECK_EQUAL(detections.size(), 1);
}


BOOST_AUTO_TEST_CASE(haar_detector_test_with_no_face)
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
    std::stringstream buffer;
    buffer << data;

    boost::property_tree::ptree detector_settings;
    boost::property_tree::read_json(buffer, detector_settings);

    auto detector = detection::create_detector(detector_settings);

    auto loaded_image = cv::imread(
            (std::filesystem::current_path() / "test_resources" / "cat_face_front_1_rgb.jpg").string(),
            cv::IMREAD_COLOR);

    auto detections = detector->detect(loaded_image);

    BOOST_CHECK_EQUAL(detections.size(), 0);
}
