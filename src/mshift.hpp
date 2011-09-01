#ifndef MSHIFT_H 
#define MSHIFT_H

#include <opencv/cv.h>

CV_IMPL void
meanShiftFiltering( cv::Mat src0, cv::Mat dst0,
    double sp0, double sr, int max_level,
    CvTermCriteria termcrit );
#endif
