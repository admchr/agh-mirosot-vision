
#include "header.h"
#include "common.hpp"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>


int main(int argc, char**argv) {
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    cv::Mat_<cv::Vec3b> img_copy = img0.clone();
    std::string out_fname = std::string(argv[3]);
    
    mirosot_vision_config config;
    init_config(&config);
    config.width = img0.size().width;
    config.height = img0.size().height;
    
    config.image = img0.ptr();
    
    load_config(&config, argv[2]);
    
    cv::Mat_<cv::Vec3b> img_white(img0.clone()), img_prescreen(img0.clone()), img_mshift(img0.clone()), img_patches(img0.clone());
    config.debug_balance = img_white.ptr();
    config.debug_meanshift = img_mshift.ptr();
    config.debug_prescreen = img_prescreen.ptr();
    config.debug_patches = img_patches.ptr();
    
    find_teams(&config);
    
    cv::imwrite(out_fname+"_0orig.png", img_copy);
    cv::imwrite(out_fname+"_1white.png", img_white);
    cv::imwrite(out_fname+"_2prescreen.png", img_prescreen);
    cv::imwrite(out_fname+"_3mshift.png", img_mshift);
    cv::imwrite(out_fname+"_4patches.png", img_patches);
    
    return 0;
}
