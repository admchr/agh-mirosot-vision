#ifndef AMV_MSHIFT_HPP
#define AMV_MSHIFT_HPP

#include "defs.hpp"

#include <opencv/cv.h>
#include <utility>

std::pair<cv::Point, cv::Vec3b> meanShiftStep(cv::Point p, cv::Vec3b color, const Image& img, int width, int height);

void meanShiftFiltering(Image src0, double sp0, double sr);
void meanShiftFilteringProportional(Image src0, double sp0, double sr);

void meanShiftPoint(Image& src0, int x, int y, double sp0, double sr);
void meanShiftPointProportional(Image& src0, int x, int y, double sp0, double sr);
#endif
