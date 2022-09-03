//
// Created by oem on 2022/9/1.
//
#include "tool.h"

void Gamma(Mat src, Mat &dst, float fGamma)
{
    unsigned char lut[256];

    for (int i = 0; i < 256; i++)
    {
        float normalize = (float)(i/255.0);
        lut[i] = saturate_cast<uchar>(pow(normalize, fGamma) * 255.0f);
    }

    src.copyTo(dst);
    MatIterator_<Vec3b> it,end;
    for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++)
    {
        (*it)[0] = lut[((*it)[0])];
        (*it)[1] = lut[((*it)[1])];
        (*it)[2] = lut[((*it)[2])];
    }

}
