#ifndef TOOLS_H_
#define TOOLS_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cmath>
#include <algorithm>


inline uchar srgb_to_rgb(uchar ch){
    double x = ch/256.0;
    double res;
    if (x>0.04045)
        res =  pow((x+0.055)/(1.055), 2.4);
    else
        res = x / 12.92;
    return std::min(255, int(res*256));
}

inline uchar rgb_to_srgb(uchar ch){
    double x = ch/256.0;
    double res;
    if (x>0.00313)
        res =  1.055*pow(x, 1/2.4) - 0.055;
    else
        res = x * 12.92;
    return std::min(255, int(res*256));
}

inline cv::Vec3b srgb_to_rgb(cv::Vec3b val){
    return cv::Vec3b(srgb_to_rgb(val[0]), srgb_to_rgb(val[1]), srgb_to_rgb(val[2]));
}
inline cv::Vec3b rgb_to_srgb(cv::Vec3b val){
    return cv::Vec3b(rgb_to_srgb(val[0]), rgb_to_srgb(val[1]), rgb_to_srgb(val[2]));
}

#endif
