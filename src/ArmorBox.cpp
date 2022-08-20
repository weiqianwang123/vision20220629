//
// Created by wang on 22-7-7.


#include "ArmorBox.h"

using namespace cv;



ArmorParam::ArmorParam()
{
    FileStorage fs("../configure/setting.xml",FileStorage::READ);
    fs["lights_angle_max_dif"] >> lights_angle_max_dif;
    fs["lights_length_max_ratio"] >> lights_length_max_ratio;
    fs["lights_Y_max_ratio"] >> lights_Y_max_ratio;
    fs["armor_width_height_min_ratio"] >> armor_width_height_min_ratio;
    fs["armor_width_height_max_ratio"] >> armor_width_height_max_ratio;
    fs["armor_max_angle"] >> armor_max_angle;
    fs["armor_inside_thresh"] >> armor_inside_thresh;

    fs["armor_number_confidence"] >> armor_number_confidence;
    fs["erase_wrong_armor"] >> erase_wrong_armor;

    fs["light_height_width_min_ratio"] >> light_height_width_min_ratio;
    fs["light_height_width_max_ratio"] >> light_height_width_max_ratio;
    fs["light_size_area_min_ratio"] >> light_size_area_min_ratio;
    fs["light_angle_to_vertigal_max"] >> light_angle_to_vertigal_max;

    fs["blue_red_diff"] >> blue_red_diff;
    fs["red_blue_diff"] >> red_blue_diff;

    fs["blue_light_threshold"] >> blue_light_threshold;
    fs["red_light_threshold"] >> red_light_threshold;

    fs["same_lights_max_dis"] >> same_lights_max_dis;
    fs["same_armors_max_dis_ratio"] >> same_armors_max_dis_ratio;
    fs["box_height_enlarge"] >> box_height_enlarge;

    fs.release();
}


LightBlob::LightBlob(cv::RotatedRect &r)
{



    rect = RotatedRect(r);
    regularRotated(rect);
    float x = rect.center.x;
    float y = rect.center.y;
    float angle = rect.angle;
    /*cout<< angle;
    cout<<" ";*/
    float height = rect.size.height / 2.0;

    if (angle < 90)
    {
        up = cv::Point(x + height * sin(angle / 180.0 * M_PI), y - height * cos(angle / 180.0 * M_PI));
        down = cv::Point(x - height * sin(angle / 180.0 * M_PI), y + height * cos(angle / 180.0 * M_PI));
    }
    else
    {
        angle = 180 - angle;
        up = cv::Point(x - height * sin(angle / 180.0 * M_PI), y - height * cos(angle / 180.0 * M_PI));
        down = cv::Point(x + height * sin(angle / 180.0 * M_PI), y + height * cos(angle / 180.0 * M_PI));
    }


}

void LightBlob::regularRotated(RotatedRect &r)
{
    if (r.size.width > r.size.height)
    {
        swap<float>(r.size.width, r.size.height);
        r.angle =r.angle >= 0.0f ? r.angle - 90.0f : r.angle + 90.f;
    }

    if (r.angle < 0)
        r.angle += 180.0f;
}


ArmorBox::ArmorBox(const LightBlob &left, const LightBlob &right, uint32_t timestamp, int box_id)
{
    light_Blobs.push_back(left);
    light_Blobs.push_back(right);

    Point center = (left.rect.center + right.rect.center) / 2.0;
    float width, height, angle;
    width = sqrt(pow(right.rect.center.x - left.rect.center.x, 2) + pow(right.rect.center.y - left.rect.center.y, 2));
    height = max(left.rect.size.height, right.rect.size.height);
    angle = atan2(right.rect.center.y - left.rect.center.y, right.rect.center.x - left.rect.center.x) * 180 / CV_PI;

    float angle_l = left.rect.angle;
    float angle_r = right.rect.angle;

    if (angle_l > 90)
        angle_l = 180 - angle_l;
    if (angle_r > 90)
        angle_r = 180 - angle_r;
    anglediff = fabs(angle_l - angle_r);


    rect = RotatedRect(center, Size(width, height), angle);


}
void ArmorBox::getPoints(std::vector<cv::Point2f> &pts)
{
    pts.clear();

    pts.push_back(light_Blobs[0].up);
    pts.push_back(light_Blobs[1].up);
    pts.push_back(light_Blobs[1].down);
    pts.push_back(light_Blobs[0].down);
}
ArmorBox::~ArmorBox(){};
LightBlob::~LightBlob(){};
