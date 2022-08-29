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
        light_rrect.size.height / light_rrect.size.width < param.light_height_width_min_ratio
            )
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


void ArmorDetector::find_light_blob(cv::Mat armor_video)
{

    Mat gray_armor_video;
    Mat sub_armor_video;
    Mat th1_armor_video;           //binary after gray
    Mat th2_armor_video;            //binary after sub
    Mat binary;
    RotatedRect rect;
    vector<Mat> channels;

    split(armor_video,channels);
    cvtColor(armor_video,gray_armor_video,COLOR_BGR2GRAY);
    threshold(gray_armor_video,th1_armor_video, param.blue_light_threshold,255,THRESH_BINARY);
    subtract(channels[0],channels[2],sub_armor_video);
    threshold(sub_armor_video,th2_armor_video,param.blue_red_diff,255,THRESH_BINARY);
    binary = th1_armor_video&th2_armor_video;

    vector<vector<Point>> contours;
    findContours(binary,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    for(int i=0;i<contours.size();i++)
    {
        if(!filterLightBlob(contours[i]))
        {
            continue;
        }
        else
        {

            rect = minAreaRect(contours[i]);
            LightBlob l(rect);
            _light_blobs.push_back(l);
            Point2f vertex[4];
            rect.points(vertex);
            for(int j=0;j<4;j++)
            {
                line(armor_video,vertex[j],vertex[(j+1)%4],Scalar(0,0,255),2,LINE_AA);
            }

        }
    }
    auto cmp = [](LightBlob a, LightBlob b) -> bool
    {
        return a.rect.center.x < b.rect.center.x;
    };
    sort(_light_blobs.begin(), _light_blobs.end(), cmp);

}


void ArmorDetector::find_armor_boxes(Mat armor_video)
{
    Solver solver;
    int p=0;
    int k=0;
    vector<Point2f> _pts;
    for(int i=0;i<_light_blobs.size();i++)
    {
        for(int j=i+1;j<_light_blobs.size();j++)
        {
            if(!isCoupleLight(_light_blobs[i],_light_blobs[j]))
                continue;
            if(isBadArmor(i,j,_light_blobs))
                continue;
            _armor_boxes.push_back(ArmorBox(_light_blobs[i],_light_blobs[j]));
            _armor_boxes[p].getPoints(_pts);
            solver.solve(_pts,SMALL);
            cout<< "pitch:"<<solver.pitch<<endl;
            cout<< "yaw:"<< solver.yaw<<endl;
            cout<<"distance:"<<solver.distance<<endl;
            Point2f vertex[4]={};
            _armor_boxes[k].rect.points(vertex);
            for(int j=0;j<4;j++)
            {
                line(armor_video,vertex[j],vertex[(j+1)%4],Scalar(0,255,0),2,LINE_AA);
            }

        }


    }
}



/*ArmorDetector::ArmorDetector(Mat video)
{
    armor_video = video;
    enemy_color = BLUE;
}*/


