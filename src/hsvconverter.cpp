#include "hsvconverter.hpp"
#include "defs.hpp"


HSVConverter::HSVConverter() {
    Image tmp(1, 1);
    for (int r=0;r<256;r++)
        for (int g=0;g<256;g++)
            for (int b=0;b<256;b++) {
                // it's damn slow
                // ... so what?
                cv::Vec3b c(b, g, r);
                tmp(0, 0) = c;
                cv::cvtColor(tmp, tmp, CV_BGR2HSV);
                table[(b<<16)|(g<<8)|(r)] = tmp(0, 0);
            }
}

void HSVConverter::convert(Image from, Image to) {
    to.create(from.size());
    for (int y=0;y<from.rows;y++)
        for (int x=0;x<from.cols;x++) {
            to(y, x) = get(from(y, x));
        }
}