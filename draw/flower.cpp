#include "methods.hpp"

cv::Mat flower(cv::Size size, cv::Point2d base, int width, int n) {
    cv::Mat canvas(size, CV_8UC4);
    canvas = 0;

    cv::Point2d center{base.x + width / 2.0, base.y + width / 2.0};

    auto radius = width / 2.0;

    std::vector<cv::Point2d> pts;
    double deg = 0;
    double degDelta = 2 * CV_PI / n;

    for (int i = 0; i < n; ++i) {
        pts.push_back({center.x + std::cos(deg) * radius, center.y + std::sin(deg) * radius});
        deg += degDelta;
    }

    for (auto pt1 : pts) {
        for (auto pt2 : pts) {
            cv::line(canvas, pt1, pt2, cv::Scalar(0,0,255,255), 1, cv::LineTypes::LINE_AA);
        }
    }

    return canvas;
}
