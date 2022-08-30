#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <string>
#include <cmath>

#include "ArmorBox.h"
#include "Armor_Detector.h"
#include "Solver.h"
#include "mercure_driver.h"

using namespace std;
using namespace cv;

int main()
{
    //camera::MercureDriver cap;
    VideoCapture  cap("../pnp_b.avi");
    Mat armor_video;

    //armor_video.create( ACQ_FRAME_HEIGHT,ACQ_FRAME_WIDTH,CV_8UC3);
    while(1)
    {
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        cap>>armor_video;
        ArmorDetector detector(armor_video);
        detector.find_light_blob(armor_video);
        detector.find_armor_boxes(armor_video);

        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
        chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
        cout << "solve time cost=" << 1/time_used.count() << "" << endl;

        imshow("armor1",armor_video);

        waitKey(1);


    }
    return 1;
}
