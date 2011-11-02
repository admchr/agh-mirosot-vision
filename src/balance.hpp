#ifndef BALANCE_H
#define BALANCE_H

#include "defs.hpp"
#include "amv.h"

#include <opencv/cv.h>
#include <vector>



cv::Vec3b median(const Image& img, amv_image_pos pos, int radius);

cv::Vec3b get_white(std::vector<std::pair<amv_image_pos, cv::Vec3b> > white_points, amv_image_pos pos);

void white_balance(Image* img, amv_config* config); 

#endif
