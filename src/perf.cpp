
#include "header.h"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

const int RUNS = 10; // TODO: this is all too slow!!!

int main(int argc, char**argv) {
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    
    mirosot_vision_conf conf;
    init_config(&conf);
    conf.width = img0.size().width;
    conf.height = img0.size().height;
    
    for (int i=0;i<RUNS;i++) {
        cv::Mat_<cv::Vec3b> img_tmp(img0);
        conf.image = img_tmp.ptr();
        find_teams(&conf);
    }
}