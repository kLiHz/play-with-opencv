#include "draw.hpp"
#include <fstream>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    putenv("QT_AUTO_SCREEN_SCALE_FACTOR=1");

    cv::Size size{1000, 1000};
    cv::Mat pic(size, CV_8UC4);
    pic = 0;

    std::string svgFile;

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

    if (command == "flower") {
        int n;
        if (!args.empty()) {
            n = std::stoi(args[0]);
        } else {
            n = 23;
        }

        auto elements = flower({50, 50}, 900, n);
        for (const auto& e : elements) {
            e->drawOn(pic);
        }
        svgFile = toSvg(size, elements);
    } 
    else if (command == "polygon") {
        int n = 6, depth = 18;
        double ratio = 0.2;
        if (!args.empty()) {
            n = std::stoi(args[0]);
            if (args.size() > 1) {
                depth = std::stoi(args[1]);
                if (args.size() > 2) {
                    ratio = std::stod(args[2]);
                }
            }
        }
        auto elements = polygon({500, 500}, {100, 260}, n, depth, ratio);
        for (const auto& e : elements) {
            e->drawOn(pic);
        }
        svgFile = toSvg(size, elements);
    }
    else {
        std::cerr << "Illegal command. Possible commands are: \n" << usage << std::endl;
        return EXIT_FAILURE;
    }

    cv::imwrite(command + ".png", pic);
    std::ofstream ofs(command + ".svg");
    ofs << svgFile;
    ofs.close();

    cv::namedWindow(command, cv::WINDOW_NORMAL);
    cv::imshow(command, pic);
    cv::waitKey();

    return EXIT_SUCCESS;
}
