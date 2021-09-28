#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "elements.hpp"

Elements flower(const cv::Point2d& base, int width, int n);

Elements polygon(const cv::Point2d& center, const cv::Point2d& a, int n, int depth, double ratio);
