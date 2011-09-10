#ifndef HSVCONVERTER_H_
#define HSVCONVERTER_H_

#include <opencv/cv.h>
#include "defs.hpp"

class HSVConverter {
public:
    HSVConverter();

    cv::Vec3b table[256*256*256];

    cv::Vec3b get(cv::Vec3b c) {
        return table[((c[0]&0xf0)<<16)|(c[1]<<8)|(c[2]&0xf0)];
    }

    void convert(Image from, Image to);
};

#endif
