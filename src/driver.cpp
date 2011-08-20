
#include "header.h"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

int main(int argc, char**argv) {
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    
    mirosot_vision_conf conf;
    conf.width = img0.size().width;
    conf.height = img0.size().height;
    conf.image = img0.ptr();
    
}