
#include "header.h"
#include "common.hpp"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>


int main(int argc, char**argv) {
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    std::string out_fname = std::string(argv[3]);
    
    mirosot_vision_config config;
    init_config(&config);
    config.width = img0.size().width;
    config.height = img0.size().height;
    
    config.image = img0.ptr();
    
    load_config(&config, argv[2]);
    
    cv::Mat_<cv::Vec3b> img_white(img0.clone()), img_mshift(img0.clone());
    config.debug_balance = img_white.ptr();
    config.debug_meanshift = img_mshift.ptr();
    
    find_teams(&config);
    
    cv::imwrite(out_fname+"_white.png", img_white);
    cv::imwrite(out_fname+"_mshift.png", img_mshift);
    
}
