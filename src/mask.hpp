#ifndef AMV_MASK_HPP
#define AMV_MASK_HPP

#include <opencv/cv.h>
#include <vector>
#include "defs.hpp"
#include "array2d.hpp"

int crossingPoint(cv::Point line1, cv::Point line2, int y);

bool pointInPolygon(cv::Point point, const std::vector<cv::Point>& polygon);

class ImageMask {
	Array2d<bool> mask;
public:
	void init(std::vector<cv::Point> polygon, cv::Size dimensions);
	void apply(Image& img);

};

#endif
