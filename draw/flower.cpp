#include <opencv2/opencv.hpp>

void draw(cv::Mat canvas, int x, int y, int k, int n) {
    auto centerX = x + k / 2.0;
    auto centerY = y + k / 2.0;
    auto radius = k / 2.0;
    std::vector<cv::Point2d> pts;
    double deg = 0;
    double degDelta = 2 * CV_PI / n;
    for (int i = 0; i < n; ++i) {
        pts.push_back({centerX + std::cos(deg) * radius, centerY + std::sin(deg) * radius});
        deg += degDelta;
    }
    for (auto pt1 : pts) {
        for (auto pt2 : pts) {
            cv::line(canvas, pt1, pt2, cv::Scalar(0,0,255), 1, cv::LineTypes::LINE_AA);
        }
    }
}

int main(int argc, char* argv[]) {
    int n;
    if (argc > 1) {
        n = std::stoi(argv[1]);
    } else {
        n = 23;
    }
    cv::Mat canvas(cv::Size(1000, 1000), CV_8UC3);
    draw(canvas, 50, 50, 900, n);
    std::string windowName = "Flower";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::imshow(windowName, canvas);
    cv::waitKey();
}
