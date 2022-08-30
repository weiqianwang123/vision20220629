//
// Created by wang on 22-7-5.
//

#ifndef INC_20220629_ARMOR_DETECTOR_H
#define INC_20220629_ARMOR_DETECTOR_H
#include "ArmorBox.h"
#include "const.h"
#include "Solver.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>



class ArmorDetector
{
    ArmorParam param;
    COLOR enemy_color;

public:
    //ArmorBox target;                             // 目标装甲板
    //ArmorBox last_target;                        // 上次识别到的目标
    ArmorBoxes _armor_boxes;                     // 目标装甲板集
    LightBlobs _light_blobs;// 目标灯条集
    Mat armor_video;
    cv::RotatedRect _res_last = cv::RotatedRect();
    cv::Rect _dect_rect = cv::Rect();



    bool filterLightBlob(const std::vector<cv::Point> &contour);





    bool isCoupleLight(const LightBlob &light_blob_i, const LightBlob &light_blob_j);

    bool isBadArmor(int i,int j,const LightBlobs &lightblobs);


    void find_light_blob(Mat armor_video);

    void find_armor_boxes(Mat armor_video);




    ArmorDetector(Mat video)
    {
        armor_video = video;
        enemy_color = BLUE;

    }

};


#endif //INC_20220629_ARMOR_DETECTOR_H



