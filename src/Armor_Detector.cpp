#include "Armor_Detector.h"
#include <math.h>

bool ArmorDetector::filterLightBlob(const std::vector<cv::Point> &contour)
{
    if (contourArea(contour) < 5)
        return false;

    RotatedRect light_rrect = minAreaRect(contour);
    LightBlob temp(light_rrect);
    light_rrect = temp.rect;

    // 筛选条件：面积，凸包比，垂直程度，高宽比
    if (contourArea(contour) / light_rrect.size.area() < param.light_size_area_min_ratio ||
        (light_rrect.angle < 90 && light_rrect.angle > param.light_angle_to_vertigal_max) ||
        (light_rrect.angle > 90 && 180 - light_rrect.angle > param.light_angle_to_vertigal_max) ||
        light_rrect.size.height / light_rrect.size.width < param.light_height_width_min_ratio)
        return false;

    return true;
}


bool ArmorDetector::isCoupleLight(const LightBlob &light_blob_i, const LightBlob &light_blob_j)
{

    float width = light_blob_j.rect.center.x - light_blob_i.rect.center.x;
    float height = (light_blob_j.rect.size.height + light_blob_j.rect.size.height) / 2.0;
    float angle_i = light_blob_i.rect.angle > 90 ? 180.0 - light_blob_i.rect.angle : light_blob_i.rect.angle;
    float angle_j = light_blob_j.rect.angle > 90 ? 180.0 - light_blob_j.rect.angle : light_blob_j.rect.angle;

    // 匹配灯条：角度差、长度比、高度差、组成的宽高比，滤除/\ \/
    if (fabs(angle_i - angle_j) > param.lights_angle_max_dif ||
        max(light_blob_i.rect.size.height, light_blob_j.rect.size.height) / min(light_blob_i.rect.size.height, light_blob_j.rect.size.height) > param.lights_length_max_ratio ||
        fabs(light_blob_i.rect.center.y - light_blob_j.rect.center.y) / (height * cos(angle_i / 180.0 * M_PI)) > param.lights_Y_max_ratio ||
        width / height > param.armor_width_height_max_ratio ||
        width / height < param.armor_width_height_min_ratio ||
        (fabs(light_blob_i.rect.angle - light_blob_j.rect.angle) >= 90 && fabs(light_blob_i.rect.angle - light_blob_j.rect.angle) < 160)) // 滤除 /\ 和 \/ 灯条
        return false;
    if(fabs(light_blob_j.up.y-light_blob_i.up.y)/fabs(light_blob_j.up.x-light_blob_i.up.x)>0.1)
        return false;

    return true;
}


/*
bool ArmorDetector::matchArmorBoxes(LightBlobs &light_blobs, ArmorBoxes &armor_boxes)
{

}
*/
bool ArmorDetector::isBadArmor(int i, int j, const LightBlobs &lightblobs)
{
    float y_min = lightblobs[i].up.y < lightblobs[j].up.y ? lightblobs[i].up.y : lightblobs[j].up.y;
    float y_max = lightblobs[i].down.y > lightblobs[j].down.y ? lightblobs[i].down.y : lightblobs[j].down.y;

    ArmorBox temp(lightblobs[i], lightblobs[j]);
    if (fabs(temp.rect.angle) > param.armor_max_angle)
        return true;

    for (int m = i + 1; m < j; m++)
    {
        float light_min = lightblobs[m].up.y;
        float light_max = lightblobs[m].down.y;
        float light_center = lightblobs[m].rect.center.y;

        if ((light_min > y_min && light_min < y_max) ||
            (light_center > y_min && light_center < y_max) ||
            (light_max > y_min && light_max < y_max) ||
            (light_min < y_min && light_max > y_max))
            return true;
    }
    return false;
}



ArmorDetector::ArmorDetector()
{
    enemy_color = BLUE;
}
