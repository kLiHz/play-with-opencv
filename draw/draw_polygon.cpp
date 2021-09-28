#include "draw.hpp"

void gen_(Elements & elements, std::vector<cv::Point2d> & oldPts, int depth, double ratio) {
    if (depth < 0) return;
    auto n = oldPts.size();
    std::vector<cv::Point2d> newPts;
    for (int i = 0; i < n; ++i) {
        auto newPoint = oldPts[i] + ratio * (oldPts[(i+1)%n] - oldPts[i]);
        newPts.push_back(newPoint);
    }
    gen_(elements, newPts, depth - 1, ratio);
    elements.push_back(std::make_shared<Polygon>(std::move(newPts)));
}

Elements polygon(const cv::Point2d& center, const cv::Point2d& a, int n, int depth, double ratio) {
    if (n < 3) return {};

    auto innerAngle = (180.0 - 360.0 / n) / CV_PI;
    std::vector<cv::Point2d> pts;

    auto dist = [](const cv::Point2d& a, const cv::Point2d& b) {
        auto d = a - b;
        return std::hypot(d.x, d.y);
    };
    auto r = dist(center, a);

    auto diff = center - a;
    auto deg = std::atan(diff.y / diff.x);
    auto degDelta = (360.0 / n) / 180.0 * CV_PI;

    for (int i = 0; i < n; ++i) {
        pts.emplace_back(center.x - r * std::cos(deg), center.y - r * std::sin(deg));
        std::cout << *(end(pts)-1) << std::endl;
        deg += degDelta;
    }

    Elements elements;
    gen_(elements, pts, depth, ratio);
    elements.emplace_back(std::make_shared<Polygon>(std::move(pts)));

    return elements;
}
