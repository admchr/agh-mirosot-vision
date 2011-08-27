#ifndef BALANCE_H
#define BALANCE_H

#include "header.h"

#include <opencv/cv.h>
#include <vector>


typedef cv::Mat_<cv::Vec3b> Image;

cv::Vec3b median(const Image& img, image_pos pos, int radius);

cv::Vec3b get_white(std::vector<std::pair<image_pos, cv::Vec3b> > white_points, image_pos pos);

void white_balance(Image* img, mirosot_vision_config* config); 

#endif
