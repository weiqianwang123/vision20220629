//
// Created by oem on 2022/9/2.
//
#include <opencv2/opencv.hpp>
#include <chrono>
using namespace std;
#ifndef INC_20220629_MAT_TIME_H
#define INC_20220629_MAT_TIME_H


class Mat_time : public cv::Mat
{
    using cv::Mat::Mat;
};
#endif //INC_20220629_MAT_TIME_H
