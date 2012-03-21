#ifndef AMV_BALANCE_HPP
#define AMV_BALANCE_HPP

#include "defs.hpp"
#include "amv.h"

#include <opencv/cv.h>
#include <vector>



WhitePoints getWhitePoints(Image& img, amv_config* config);
cv::Vec3b median(const Image& img, cv::Point pos, int radius);
cv::Vec3b get_white(WhitePoints white_points, cv::Point pos);
void white_balance(Image& img, const WhitePoints& whitePoints);

#endif
