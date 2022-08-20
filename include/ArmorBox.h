//
// Created by wang on 22-7-7.
//

#ifndef INC_20220629_ARMORBOX_H
#define INC_20220629_ARMORBOX_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include "const.h"
using namespace cv;
using namespace std;

typedef struct ArmorParam
{
    float lights_angle_max_dif;                 // 灯条对角度差最大值
    float lights_length_max_ratio;              // 灯条对长度比最大值
    float lights_Y_max_ratio;                   // Y坐标差 / 平均长度
    float armor_width_height_min_ratio;         // 装甲板宽高比最小值
    float armor_width_height_max_ratio;         // 装甲板宽高比最大值
    float armor_max_angle;                      // 装甲板最大倾斜角度
    float armor_inside_thresh;                  // 装甲板区域内的最大亮度

    bool erase_wrong_armor;                     // 是否滤除错误数字目标
    float armor_number_confidence;              // 分类器置信度

    float light_height_width_min_ratio;         // 灯条高宽比最小值
    float light_height_width_max_ratio;         // 灯条高宽比最大值
    float light_size_area_min_ratio;            // 灯条最小凸包比
    float light_angle_to_vertigal_max;          // 灯条角度与90°差值的最大值

    float blue_red_diff;                        // B - R
    float red_blue_diff;                        // R - B

    float blue_light_threshold;                 // 蓝色灯条所设阈值
    float red_light_threshold;                  // 红色灯条所设阈值

    float same_lights_max_dis;                  // 判断相同灯条的距离差值
    float same_armors_max_dis_ratio;                  // 判断相同装甲板的距离差值

    float box_height_enlarge;                   // 装甲板高度放大系数

    ArmorParam();
}ArmorParam;



class LightBlob
{
public:
    RotatedRect rect;
    Point up;
    Point down;

    LightBlob(cv::RotatedRect &r);
    ~LightBlob();

    void regularRotated(cv::RotatedRect &r);

};
typedef std::vector<LightBlob> LightBlobs;



class ArmorBox
{
public:
    LightBlobs light_Blobs;
    RotatedRect rect;
    Rect box;
    float anglediff;

    void getPoints(std::vector<cv::Point2f> &);

    ArmorBox(const LightBlob &left, const LightBlob &right, uint32_t timestamp = 0, int box_id = 10);
    ~ArmorBox();
};
typedef std::vector<ArmorBox> ArmorBoxes;
#endif //INC_20220629_ARMORBOX_H





