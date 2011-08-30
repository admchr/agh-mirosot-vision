#ifndef MSHIFT_H 
#define MSHIFT_H

#include <opencv/cv.h>
CV_IMPL void
meanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
                         double sp0, double sr, int max_level,
                         CvTermCriteria termcrit );

#endif