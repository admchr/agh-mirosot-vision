#ifndef AMV_MOMENTS_HPP
#define AMV_MOMENTS_HPP

#include <opencv/cv.h>
#include <vector>

class PatchMoments {
    int count;
    double sum_x;
    double sum_y;
    double sum_xx;
    double sum_xy;
    double sum_yy;
    cv::Moments getMoments();
public:
    PatchMoments();
    void add(cv::Point p);
    cv::Point2d getMean();
    int getCount();
    double getXVariance();
    double getYVariance();
    double getCovariance();
    double getDeterminant();
    double getAngle();
    double getRegressionSlope();
    double getRegressionPosition();
};
#endif
