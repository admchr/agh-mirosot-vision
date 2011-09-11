#ifndef MOMENTS_H
#define MOMENTS_H

#include <opencv/cv.h>
#include <vector>

class PatchMoments {
    int count;
    double sum_x;
    double sum_y;
    double sum_xx;
    double sum_xy;
    double sum_yy;
    std::vector<cv::Point> points;
    cv::Moments getMoments();
public:
    PatchMoments();
    void add(cv::Point p);
    cv::Point getMean();
    int getCount();
    double getXVariance();
    double getYVariance();
    double getCovariance();
    double getAngle();
};
#endif
