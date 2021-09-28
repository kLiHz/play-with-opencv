#include "methods.hpp"
#include <stdlib.h>

int main(int argc, char* argv[]) {

    putenv("QT_AUTO_SCREEN_SCALE_FACTOR=1");

    std::string usage = 
    "./draw flower [edges=23]\n"
    "./draw polygon [edges=6, [depth=18, [ratio=0.2]]]\n";

    if (argc < 2) {
        std::cerr << "No drawing command. Possible usages are:\n" << usage << std::endl;
        return EXIT_FAILURE;
    }

    auto command = std::string(argv[1]);

    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.emplace_back(std::string(argv[i]));
    }

    cv::Mat result;

    if (command == "flower") {
        int n;
        if (args.size() > 1) {
            n = std::stoi(args[0]);
        } else {
            n = 23;
        }
        result = flower({1000, 1000}, {50, 50}, 900, n);
    } 
    else if (command == "polygon") {
        int n = 6, depth = 18;
        double ratio = 0.2;
        if (args.size() > 0) {
            n = std::stoi(args[0]);
            if (args.size() > 1) {
                depth = std::stoi(args[1]);
                if (args.size() > 2) {
                    ratio = std::stod(args[2]);
                }
            }
        }
        result = polygon({1000, 1000}, {500, 500}, {100, 260}, n, depth, ratio);
    }
    else {
        std::cerr << "Illegal command. Possible commands are: \n" << usage << std::endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow(command, cv::WINDOW_NORMAL);
    cv::imshow(command, result);
    cv::imwrite(command + ".png", result);
    cv::waitKey();
}
