#include "hsvconverter.hpp"
#include "defs.hpp"

using namespace cv;

HSVConverter::HSVConverter() {
    Image tmp(1, 1);
    for (int b=0; b<256; b++)
        for (int g=0; g<256; g++)
            for (int r=0; r<256; r++)
                {
                Vec3b c;
                int min_v = r;
                if (g < min_v) min_v = g;
                if (b < min_v) min_v = b;

                int max_v = r;
                if (g > max_v) max_v = g;
                if (b > max_v) max_v = b;

                int sat = max_v - min_v;
                if (sat == 0) {
                    c[0] = 0;
                } else if (max_v == r) {
                    c[0] = ((g - b)*30/sat + 180)%180;
                } else if (max_v == g) {
                    c[0] = (b - r)*30/sat + 60;
                } else {
                    c[0] = (r - g)*30/sat + 120;
                }
                c[1] = sat;
                c[2] = (r + g + b)/3;
                /*
                // it's damn slow
                // but is done only once
                Vec3b c(b, g, r);
                tmp(0, 0) = c;
                cv::cvtColor(tmp, tmp, CV_BGR2HSV);
                c = tmp(0, 0);
                c[2] = (r+g+b)/3;
                */
                table[(b<<16)|(g<<8)|(r)] = c;

            }
}

void HSVConverter::fromBGRToHSL(Image from, Image to) {
    to.create(from.size());
    for (int y=0; y<from.rows; y++)
        for (int x=0; x<from.cols; x++) {
            to(y, x) = fromBGRToHSL(from(y, x));
        }
}

Vec3b HSVConverter::fromHSVToBGR(Vec3b hsv) {
    Image tmp(1, 1);
    tmp(0, 0) = hsv;
    cv::cvtColor(tmp, tmp, CV_HSV2BGR);
    return tmp(0, 0);
}

HSVConverter hsvconverter;
