#include <string>
#include <format>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class Element {
public:
    virtual std::string toSvgElement() = 0;
    virtual void drawOn(cv::Mat canvas) = 0;
};

class Line : public Element {
    cv::Point2d x;
    cv::Point2d y;
public:
    Line(cv::Point2d x_, cv::Point2d y_) : x(x_), y(y_) {}
    std::string toSvgElement() final;
    void drawOn(cv::Mat canvas) final;
};

class Polygon : public Element {
    std::vector<cv::Point2d> pts;
public:
    Polygon(std::vector<cv::Point2d> pts_) : pts(std::move(pts_)) {}
    std::string toSvgElement() final;
    void drawOn(cv::Mat canvas) final;
};



using Elements = std::vector<std::shared_ptr<Element>>;

inline std::string toSvg(cv::Size size, Elements v) {
    std::string header = R"(<svg version="1.1" width="{}" height="{}" xmlns="http://www.w3.org/2000/svg">)";
    std::string ending = "</svg>";
    return std::format(header, size.width, size.height) +
        std::accumulate(begin(v), end(v), std::string(""), [](std::string s, std::shared_ptr<Element> e) {
            return std::move(s) + e->toSvgElement();
        }) + ending;
}
