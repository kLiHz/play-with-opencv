#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat flower(cv::Size size, cv::Point2d base, int width, int n);

cv::Mat polygon(cv::Size size, cv::Point2d center, cv::Point2d a, int n, int depth, double ratio);
