#ifndef PIXEL_H
#define PIXEL_H

#include <opencv/cv.h>

class PixelSet {
	int count;
public:
	PixelSet() {
	    count = 0;
	}
	void add(cv::Point p, cv::Vec3b color){
	    count++;
	}
	int getCount() {
	    return count;
	}
};

#endif
