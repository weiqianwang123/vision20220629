//
// Created by wang on 22-7-5.
//

#ifndef INC_20220629_ARMOR_DETECTOR_H
#define INC_20220629_ARMOR_DETECTOR_H
#include "ArmorBox.h"
#include "const.h"



class ArmorDetector
{
    ArmorParam param;
    COLOR enemy_color;

public:
    //ArmorBox target;                             // 目标装甲板
    //ArmorBox last_target;                        // 上次识别到的目标
    ArmorBoxes _armor_boxes;                     // 目标装甲板集
    LightBlobs _light_blobs;                     // 目标灯条集



    bool filterLightBlob(const std::vector<cv::Point> &contour);


    bool matchArmorBoxes(LightBlobs &light_blobs, ArmorBoxes &armor_boxes);

    bool isCoupleLight(const LightBlob &light_blob_i, const LightBlob &light_blob_j);

    bool isBadArmor(int i,int j,const LightBlobs &lightblobs);


    ArmorDetector();

};


#endif //INC_20220629_ARMOR_DETECTOR_H



