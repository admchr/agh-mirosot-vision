#ifndef PIXEL_H
#define PIXEL_H

#include "header.h"
#include <opencv/cv.h>

class PixelSet {
	int count;
public:
	PixelSet() {
	    count = 0;
	}
	void add(cv::Point p, cv::Vec3b color);
	int getCount();

	bool meetsRobotCriteria(mirosot_vision_config* conf);
};

#endif
