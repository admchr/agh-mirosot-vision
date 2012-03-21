#ifndef AMV_DEFS_HPP
#define AMV_DEFS_HPP

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#include <opencv/cv.h>

typedef cv::Mat_<cv::Vec3b> Image;
typedef std::vector<std::pair<cv::Point, cv::Vec3b> > WhitePoints;

#endif
