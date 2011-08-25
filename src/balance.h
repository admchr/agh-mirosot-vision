#ifndef BALANCE_H
#define BALANCE_H
#include <opencv/cv.h>

#include "header.h"

typedef cv::Mat_<cv::Vec3b> Image;

cv::Vec3b median(const Image& img, image_pos pos, int radius);

cv::Vec3b get_average(image_pos* white_points, int white_points_len, image_pos pos);

void white_balance(Image* img, mirosot_vision_config* config); 

#endif
