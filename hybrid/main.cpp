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

int main(int argc, char *argv[])
{
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
        parser.printErrors();
        return 0;
    }

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

    cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);

    auto L1 = buildLaplacianPyramid(img1, n);
    auto L2 = buildLaplacianPyramid(img2, n);

    // merging

    std::vector<cv::Mat> L3;

    for (auto i = 0; i <= n; ++i) {
        L3.push_back(
            (0.5 * L1[i]) + (0.5 * L2[i])
        );
    }
    
    // reconstruct

    auto result = L3[n];

    for (auto i = n - 1; i >= 0; --i) {
        result = pyrUp(result) + L3[i];
    }

    if (visual) {
        for (int i = 0; i <= n; ++i) {
            auto winName1 = fmt::format("{}: L[{}]", imgPath1, i);
            auto winName2 = fmt::format("{}: L[{}]", imgPath2, i);
            cv::namedWindow(winName1, cv::WINDOW_NORMAL);
            cv::namedWindow(winName2, cv::WINDOW_NORMAL);
            cv::imshow(winName1, L1[i]);
            cv::imshow(winName2, L2[i]);
        }
        cv::namedWindow("result", cv::WINDOW_NORMAL);
        cv::imshow("result", result);
        cv::waitKey(0);
    }

    if (verbose) {
        for (int i = 0; i <= n; ++i) {
            auto pattern = "./hybrid-out/{}/L_{}.png";
            cv::imwrite(fmt::format(pattern, "img1", i), L1[i]);
            cv::imwrite(fmt::format(pattern, "img2", i), L2[i]);
        }
    }
    cv::imwrite(imgPath3, result);
    return 0;
}
