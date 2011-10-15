
#include "amv.h"
#include "common.hpp"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>
#include <iostream>

const int RUNS = 1000;

int main(int argc, char**argv) {
    if (argc != 3) {
        std::cerr<<"usage: ./"<<argv[0]<<" image config"<<std::endl;
        return 1;
    }
    cv::Mat_<cv::Vec3b> img0 = cv::imread(std::string(argv[1]));
    
    amv_config config;
    amv_config_init(&config);
    config.width = img0.size().width;
    config.height = img0.size().height;
    load_config(&config, argv[2]);
    
    amv_state* state = amv_state_new(config);
    
    std::cout<<RUNS<<" iterations "<<std::endl;
    for (int i=0;i<RUNS;i++) {
        cv::Mat_<cv::Vec3b> img_tmp(img0);
        amv_find_teams(img_tmp.ptr(), state, 0);
    }

    amv_state_free(state);

    return 0;
}
