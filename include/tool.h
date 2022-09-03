//
// Created by oem on 2022/9/1.
//

#ifndef INC_20220629_TOOL_H
#define INC_20220629_TOOL_H
#include <eigen3/Eigen/Dense>
#include <opencv2/opencv.hpp>
using namespace cv;

/**
 *  @brief  Gamma算法，用于调整图片的明暗程度
 *  @param  src 原图像
 *  @param  dst 输出图像
 *  @param  fGamma  gamma参数，(大于1.0时整体变亮，小于1.0时整体变暗)
 */
void Gamma(Mat src, Mat &dst, float fGamma);



#endif //INC_20220629_TOOL_H
