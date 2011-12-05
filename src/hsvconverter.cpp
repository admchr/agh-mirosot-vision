#include "hsvconverter.hpp"
#include "defs.hpp"

using namespace cv;

HSVConverter::HSVConverter() {
    Image tmp(1, 1);
    for (int r=0;r<256;r++)
        for (int g=0;g<256;g++)
            for (int b=0;b<256;b++) {
                // it's damn slow
                // but is done only once
                Vec3b c(b, g, r);
                tmp(0, 0) = c;
                cv::cvtColor(tmp, tmp, CV_BGR2HSV);
                c = tmp(0, 0);
                c[2] = (r+g+b)/3;
                table[(b<<16)|(g<<8)|(r)] = c;
            }
}

void HSVConverter::convert(Image from, Image to) {
    to.create(from.size());
    for (int y=0;y<from.rows;y++)
        for (int x=0;x<from.cols;x++) {
            to(y, x) = get(from(y, x));
        }
}

Vec3b HSVConverter::getBGR(Vec3b hsv) {
    Image tmp(1, 1);
    tmp(0, 0) = hsv;
    cv::cvtColor(tmp, tmp, CV_HSV2BGR);
    return tmp(0, 0);
}

HSVConverter hsvconverter;
