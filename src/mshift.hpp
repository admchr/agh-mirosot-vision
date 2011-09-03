#ifndef MSHIFT_H 
#define MSHIFT_H

#include "defs.hpp"

#include <opencv/cv.h>
// TODO: where is std::pair?
#include <map>

std::pair<cv::Point, cv::Vec3b> meanShiftStep(cv::Point p, cv::Vec3b color, const Image& img, int width, int height);

CV_IMPL void
meanShiftFiltering( Image src0, Image dst0,
    double sp0, double sr, int max_level,
    CvTermCriteria termcrit );
#endif
