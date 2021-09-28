#include <iostream>
#include <opencv2/highgui.hpp>

int main(int argc, char *argv[])
{
    std::string filename;
    if (argc == 2) {
        filename = argv[1];
    } else {
        std::cout << "Input Filename: ";
        std::cin >> filename;
    }

    cv::Mat img = cv::imread(filename, cv::ImreadModes::IMREAD_UNCHANGED);

    if (img.empty()) {
        std::cout << "Unable to open '" << filename << "'." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Image loaded successfully." << std::endl;

    cv::namedWindow(filename, cv::WINDOW_NORMAL);
    cv::imshow(filename, img);
    cv::waitKey(0);

    return 0;
}
