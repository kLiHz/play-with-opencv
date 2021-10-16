#include <opencv2/opencv.hpp>

// wrapper for convenience
auto pyrUp(cv::Mat const & a) {
    cv::Mat dst;
    cv::pyrUp(a, dst);
    return dst;
}

// wrapper for convenience
auto pyrDown(cv::Mat const & a) {
    cv::Mat dst;
    cv::pyrDown(a, dst);
    return dst;
}

// wrapper for convenience
auto buildGaussianPyramid(cv::Mat const & a, int maxLevel) {
    // G[0] is the same as a;
    std::vector<cv::Mat> G;
    cv::buildPyramid(a, G, maxLevel);
    return G;
}

auto buildLaplacianPyramid(std::vector<cv::Mat> const & G) {
    std::vector<cv::Mat> L( G.size() );
    for (int i = 0; i < G.size() - 1; ++i) {
        auto expanded = pyrUp(G[i+1]);
        L[i] = G[i] - expanded;
    }
    L[G.size()-1] = G[G.size()-1];
    return L;
}

auto buildLaplacianPyramid(cv::Mat const & a, int maxLevel) {
    auto G = buildGaussianPyramid(a, maxLevel);
    return buildLaplacianPyramid(G);
}

#include <fmt/core.h>

using MatVector = std::vector<cv::Mat>;

std::tuple<cv::Mat, MatVector, MatVector> 
getLinearHybridImage(cv::Mat const & img1, double a, cv::Mat const & img2, double b, int maxLayerLevel) {
    auto L1 = buildLaplacianPyramid(img1, maxLayerLevel);
    auto L2 = buildLaplacianPyramid(img2, maxLayerLevel);

    std::vector<cv::Mat> L3;

    for (auto i = 0; i <= maxLayerLevel; ++i) {
        L3.push_back(
            (a * L1[i]) + (b * L2[i])
        );
    }

    // reconstruct
    auto result = L3[maxLayerLevel];
    for (auto i = maxLayerLevel - 1; i >= 0; --i) {
        result = pyrUp(result) + L3[i];
    }

    return {result, L1, L2};
}

#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    _putenv("QT_AUTO_SCREEN_SCALE_FACTOR=1");

    auto keys = 
        "{help h ?  |       | Print this message               }"
        "{@image1   |<none> | First image's path               }"
        "{@image2   |<none> | Second image's path              }"
        "{@image3   |out.png| Hybrid image's path              }"
        "{layers n  |3      | Max pyramid level; default to 3  }"
        "{visual    |       | Use imshow to visualize pyramids }"
        "{verbose   |       | Write out layers of pyramids     }";
    
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Image hybrid v1.0.0");

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    auto n = parser.get<int>("n");          // Max layer level
    auto verbose = parser.has("verbose");   // Write out layers of pyramids
    auto visual = parser.has("visual");     // Use imshow to visualize pyramids

    auto imgPath1 = parser.get<std::string>("@image1");
    auto imgPath2 = parser.get<std::string>("@image2");
    auto imgPath3 = parser.get<std::string>("@image3");

    if (!parser.check()) {
        parser.printMessage();
        parser.printErrors();
        return 0;
    }

    std::cout 
        << std::boolalpha
        << "Max layer number: " << n << "\n"
        << "Total pyramid layers: " << n + 1 << "\n"
        << "Write out layers of pyramids: " << verbose << "\n"
        << "Use imshow to visualize pyramids: " << visual << "\n";

    auto img1 = cv::imread(imgPath1);
    auto img2 = cv::imread(imgPath2);

    if (img1.empty()) {
        std::cout << "Unable to open '" << imgPath1 << "'." << std::endl;
        return 1;
    }

    if (img2.empty()) {
        std::cout << "Unable to open '" << imgPath2 << "'." << std::endl;
        return 1;
    }

    std::cout << "Image loaded successfully." << std::endl;

    cv::Mat result;

    auto viewLapPyr = [&](MatVector const & L1, MatVector const & L2, std::string winNamePrefix = "") {
        for (int i = 0; i <= n; ++i) {
            auto winName1 = fmt::format("{} {}: L[{}]", winNamePrefix, imgPath1, i);
            auto winName2 = fmt::format("{} {}: L[{}]", winNamePrefix, imgPath2, i);
            cv::namedWindow(winName1, cv::WINDOW_NORMAL);
            cv::namedWindow(winName2, cv::WINDOW_NORMAL);
            cv::imshow(winName1, L1[i]);
            cv::imshow(winName2, L2[i]);
        }
        cv::waitKey(0);
        cv::destroyAllWindows();
    };

    auto wirteOutLapPyr = [&](MatVector const & L1, MatVector const & L2) {
        for (int i = 0; i <= n; ++i) {
            fs::create_directory("./hybrid-out/");
            fs::create_directory("./hybrid-out/img1/");
            fs::create_directory("./hybrid-out/img2/");
            cv::imwrite(fmt::format("./hybrid-out/img1/L_{}.png", i), L1[i]);
            cv::imwrite(fmt::format("./hybrid-out/img2/L_{}.png", i), L2[i]);
        }
    };

    if (img1.channels() != img2.channels()) {
        if (img1.channels() != 1) {
            cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
            cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);
        }
        auto [result, L1, L2] = getLinearHybridImage(img1, 0.5, img2, 0.5, n);
        if (visual) viewLapPyr(L1, L2);
        if (verbose) wirteOutLapPyr(L1, L2);
    } else {
        std::vector<cv::Mat> split1, split2, split3;
        cv::split(img1, split1);
        cv::split(img2, split2);
        split3.resize(img1.channels());
        for (int i = 0; i < split1.size(); ++i) {
            auto [a, L1, L2] = getLinearHybridImage(split1[i], 0.5, split2[i], 0.5, n);
            split3[i] = a;
            if (visual) viewLapPyr(L1, L2, fmt::format("Channel {}:", i));
        }
        cv::merge(split3, result);
        
        cv::merge(split1, img1);
        cv::merge(split2, img2);

        // Got L1, L2 from GRAY image; compromised solution
        cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);
        auto [r, L1, L2] = getLinearHybridImage(img1, 0.5, img2, 0.5, n);
        if (verbose) wirteOutLapPyr(L1, L2);
    }

    if (visual) {
        cv::namedWindow("result", cv::WINDOW_NORMAL);
        cv::imshow("result", result);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    cv::imwrite(imgPath3, result);
    return 0;
}
