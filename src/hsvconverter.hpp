#ifndef AMV_HSVCONVERTER_HPP
#define AMV_HSVCONVERTER_HPP

#include <opencv/cv.h>
#include "defs.hpp"

class HSVConverter {
public:
    HSVConverter();

    cv::Vec3b table[256*256*256];

    cv::Vec3b fromBGRToHSL(cv::Vec3b c) {
        return table[((c[0])<<16)|(c[1]<<8)|(c[2])];
    }

    void fromBGRToHSL(Image from, Image to);

    cv::Vec3b fromHSVToBGR(cv::Vec3b hsv);
};
extern HSVConverter hsvconverter;
#endif
