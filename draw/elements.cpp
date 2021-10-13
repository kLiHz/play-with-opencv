#include "elements.hpp"

std::string Polygon::toSvgElement() {
    return R"(<polygon fill="none" points=")" +
           std::accumulate(
                   begin(pts), end(pts), std::string(""),
                   [](std::string s, const cv::Point2d& p){
                       return std::move(s) + fmt::format("{:.2f}, {:.2f} ", p.x, p.y);}) + "\"/>";
}

void Polygon::drawOn(cv::Mat canvas) {
    auto n = pts.size();
    for (int i = 0; i < n; ++i) {
        cv::line(canvas, pts[i], pts[(i+1)%n],
                 cv::Scalar(0,0,255,255), 1, cv::LineTypes::LINE_AA);
    }
}

std::string Line::toSvgElement() {
    return fmt::format(
            R"(<line x1="{:.2f}" x2="{:.2f}" y1="{:.2f}" y2="{:.2f}"/>)",
            x.x, y.x, x.y, y.y);
}

void Line::drawOn(cv::Mat canvas) {
    cv::line(canvas, x, y,
             cv::Scalar(0,0,255,255), 1, cv::LineTypes::LINE_AA);
}
