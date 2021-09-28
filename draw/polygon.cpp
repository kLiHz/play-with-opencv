#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void draw_by_pts(cv::Mat canvas, std::vector<cv::Point2d> pts) {
    auto n = pts.size();
    for (int i = 0; i < n; ++i) {
        cv::line(canvas, pts[i], pts[(i+1)%n], cv::Scalar(0,0,255,255), 1, cv::LineTypes::LINE_AA);
    }
}

void draw_(cv::Mat canvas, std::vector<cv::Point2d> oldPts, int depth, double ratio) {
    if (depth < 0) return;
    auto n = oldPts.size();
    std::vector<cv::Point2d> newPts;
    for (int i = 0; i < n; ++i) {
        auto newPoint = oldPts[i] + ratio * (oldPts[(i+1)%n] - oldPts[i]);
        newPts.push_back(newPoint);
    }
    draw_by_pts(canvas, newPts);
    draw_(canvas, newPts, depth - 1, ratio);
}

cv::Mat polygon(cv::Size size, cv::Point2d center, cv::Point2d a, int n, int depth, double ratio) {
    cv::Mat canvas(size, CV_8UC4);
    canvas = 0;

    if (n < 3) return canvas;

    auto innerAngle = (180.0 - 360.0 / n) / CV_PI;
    std::vector<cv::Point2d> pts;

    auto dist = [](cv::Point2d a, cv::Point2d b) {
        auto d = a - b;
        return std::hypot(d.x, d.y);
    };
    auto r = dist(center, a);

    auto diff = center - a;
    auto deg = std::atan(diff.y / diff.x);
    auto degDelta = (360.0 / n) / 180.0 * CV_PI;

    for (int i = 0; i < n; ++i) {
        pts.push_back({center.x - r * std::cos(deg), center.y - r * std::sin(deg)});
        std::cout << *(end(pts)-1) << std::endl;
        deg += degDelta;
    }

    draw_by_pts(canvas, pts);
    draw_(canvas, pts, depth, ratio);

    return canvas;
}

int main(int argc, char* argv[]) {
    int n = 6, depth = 10;
    double ratio = 0.2;

    if (argc > 1) {
        n = std::stoi(argv[1]);
        if (argc > 2) {
            depth = std::stoi(argv[2]);
            if (argc > 3) {
                ratio = std::stod(argv[3]);
            }
        }
    }
    
    auto pic = polygon(cv::Size(1000, 1000), cv::Point2d(500, 500), cv::Point2d(100, 250), n, depth, ratio);
    std::string windowName = "Polygon";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::imshow(windowName, pic);
    cv::imwrite(windowName + ".png", pic);
    cv::waitKey();
}
​