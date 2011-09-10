#ifndef MOMENTS_H
#define MOMENTS_H

#include <opencv/cv.h>
#include <vector>

class PatchMoments {
    int count;
    int sum_x;
    int sum_y;
    std::vector<cv::Point> points;
    cv::Moments getMoments();
public:
    PatchMoments();
    void add(cv::Point p);
    cv::Point getMean();
    int getCount();
};
#endif
