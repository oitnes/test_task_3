#include "detector/detector_factory.hpp"

#include <boost/test/unit_test.hpp>
#include <opencv2/imgcodecs.hpp>


BOOST_AUTO_TEST_CASE(haar_detector_test)
{
    auto detector = detection::create_detector(
            std::filesystem::current_path() / "test_resources" / "haar_detector_description.json");

    auto loaded_image = cv::imread(
            (std::filesystem::current_path() / "test_resources" / "face_front_1_rgb.bmp").string(),
            cv::IMREAD_COLOR);
    auto detections = detector->detect(loaded_image);

    BOOST_CHECK_EQUAL(detections.size(), 1);
}
