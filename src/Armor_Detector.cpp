#include "Armor_Detector.h"
#include <math.h>
#include <chrono>
#include <string.h>

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
    const cv::Point & last_result = _res_last.center;
    if(last_result.x == 0 || last_result.y == 0){
        _dect_rect = Rect(0, 0, armor_video.cols, armor_video.rows);
    }
    else
    {
        Rect rect = _res_last.boundingRect();
        double scale_w = 1.3 + 0.7;
        double scale_h = 2;
        int w = int(rect.width * scale_w);
        int h = int(rect.height * scale_h);
        Point center = last_result;
        int x = std::max(center.x - w, 0);
        int y = std::max(center.y - h, 0);
        Point lu = Point(x, y);
        x = std::min(center.x + w, armor_video.cols);
        y = std::min(center.y + h, armor_video.rows);
        Point rd = Point(x, y);


        _dect_rect = Rect(lu, rd);
        armor_video(_dect_rect).copyTo(armor_video);
    }
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
    //Point point = (20);
    int p=0;
    int k=0;
    string string;
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
            _res_last = _armor_boxes[p].rect;
            solver.solve(_pts,SMALL);
            cout<< "pitch:"<<solver.pitch<<endl;
            cout<< "yaw:"<< solver.yaw<<endl;
            cout<<"distance:"<<solver.distance<<endl;
            try
            {
                getArmorNum(_armor_boxes);
            }
            catch(std::bad_cast)
            {
                cout<< "error!"<<endl;
            }



            //cout<<_armor_boxes[p].id<<endl;

            string = to_string(_armor_boxes[p].id);
            p++;
            //putText("armor1",string,point,cv::FONT_HERSHEY_SIMPLEX,0.45, CV_RGB(255,230,0),1.8);
            Point2f vertex[4]={};
            _armor_boxes[k].rect.points(vertex);
            k++;
            for(int j=0;j<4;j++)
            {
                line(armor_video,vertex[j],vertex[(j+1)%4],Scalar(0,255,0),2,LINE_AA);
            }

        }


    }

}

bool ArmorDetector::getArmorNum(ArmorBoxes &armor_boxes)
{
    Mat temp;

    for (auto &armor : armor_boxes) {
        adjustBox(armor.box);
        cout << "OK: " << __LINE__ << endl;
        temp = src(Rect2d(armor.box));
        resize(temp, temp, Size(28, 28), cv::INTER_LINEAR);
        Gamma(temp, temp, 0.6);
        cvtColor(temp, temp, COLOR_BGR2GRAY);
        armor.id = classifier(temp);
        armor.confidence = 1.0;
        cout<<armor.id<<endl;
    }
}


void ArmorDetector::adjustBox(Rect &r)
{
    r.y -= r.height / 2.0 * param.box_height_enlarge;
    r.height *= 2.0 * param.box_height_enlarge;
    r &= Rect(Point(0, 0), src.size());
}



/*
ArmorDetector::ArmorDetector(Mat video)
{

    classifier = Classifier("../Configure/param/");
}
*/


