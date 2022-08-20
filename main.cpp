#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <time.h>
#include <iostream>
#include <string>
#include <cmath>

#include "ArmorBox.h"
#include "Armor_Detector.h"
#include "Solver.h"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture capture;
    capture.open("../pnp_b.avi");
    int sec=0;
    int g_sec = 0;
    double t=0;
    while(1)
    {
        int k=0;
        int p=0;
        vector<Point2f> _pts;
        t=(double)getTickCount() ;
        Solver solver;
        ArmorParam param;
        RotatedRect rect;
        ArmorDetector detector;
        Mat armor_video;
        Mat gray_armor_video;
        Mat sub_armor_video;
        Mat th1_armor_video;           //binary after gray
        Mat th2_armor_video;            //binary after sub
        Mat binary;
        vector<Mat> channels;
        capture>>armor_video;
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
            if(!detector.filterLightBlob(contours[i]))
            {
                continue;
            }
            else
            {

                rect = minAreaRect(contours[i]);
                LightBlob l(rect);
                detector._light_blobs.push_back(l);
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
        sort(detector._light_blobs.begin(), detector._light_blobs.end(), cmp);
        for(int i=0;i<detector._light_blobs.size();i++)
        {
            for(int j=i+1;j<detector._light_blobs.size();j++)
            {
               if(!detector.isCoupleLight(detector._light_blobs[i],detector._light_blobs[j]))
                    continue;
               if(detector.isBadArmor(i,j,detector._light_blobs))
                    continue;
                detector._armor_boxes.push_back(ArmorBox(detector._light_blobs[i],detector._light_blobs[j]));
                detector._armor_boxes[p].getPoints(_pts);
                solver.solve(_pts,SMALL);
                cout<< "pitch:"<<solver.pitch<<endl;
                cout<< "yaw:"<< solver.yaw<<endl;
                cout<<"distance:"<<solver.distance<<endl;
                Point2f vertex[4]={};
                detector._armor_boxes[k].rect.points(vertex);
                for(int j=0;j<4;j++)
                {
                    line(armor_video,vertex[j],vertex[(j+1)%4],Scalar(0,255,0),2,LINE_AA);
                }

            }


        }
        imshow("armor2",binary);
        imshow("armor1",armor_video);

        waitKey(100);

        /*t=(double)getTickCount()-t / getTickFrequency();
        cout<< t;*/
    }
    return 1;
}
