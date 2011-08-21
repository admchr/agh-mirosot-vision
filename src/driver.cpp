
#include "header.h"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

int main(int argc, char**argv) {
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    std::string out_fname = std::string(argv[2]);
    
    mirosot_vision_conf conf;
    init_config(&conf);
    conf.width = img0.size().width;
    conf.height = img0.size().height;
    
    conf.image = img0.ptr();
    
    cv::Mat_<cv::Vec3b> img_white(img0.clone()), img_mshift(img0.clone());
    conf.debug_balance = img_white.ptr();
    conf.debug_meanshift = img_mshift.ptr();
    
    find_teams(&conf);
    
    cv::imwrite(out_fname+"_white.png", img_white);
    cv::imwrite(out_fname+"_mshift.png", img_mshift);
    
}