#include "detection/detector.hpp"

#include <boost/program_options.hpp>

#include <opencv2/imgcodecs.hpp>

#include <string>
#include <iostream>

namespace po = boost::program_options;


int main(int argc, const char **argv) {
    po::options_description desc("Evaluation options");
    std::string haar_cascades_path;
    std::string image_path;
    desc.add_options()
            ("help,h", "Show help")
            ("haars_path,s", po::value<std::string>(&haar_cascades_path), "set haar cascades paths")
            ("image_path,p", po::value<std::string>(&image_path), "set image paths")
            ;
    detection::FaceDetector detector(haar_cascades_path, 1);
    auto image = imread(image_path, cv::IMREAD_COLOR);
    if(image.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return EXIT_FAILURE;
    }
    auto detection_result = detector.detect(image);

    return EXIT_SUCCESS;
}
