#include "draw.hpp"

Elements flower(const cv::Point2d& base, int width, int n) {

    cv::Point2d center{base.x + width / 2.0, base.y + width / 2.0};

    auto radius = width / 2.0;

    std::vector<cv::Point2d> pts;
    double deg = 0;
    double degDelta = 2 * CV_PI / n;

    for (int i = 0; i < n; ++i) {
        pts.emplace_back(center.x + std::cos(deg) * radius, center.y + std::sin(deg) * radius);
        deg += degDelta;
    }

    Elements elements;

    for (auto & pt1 : pts) {
        for (auto & pt2 : pts) {
            if (pt1 == pt2) continue;
            elements.emplace_back(std::make_shared<Line>(pt1, pt2));
        }
    }

    return elements;
}
