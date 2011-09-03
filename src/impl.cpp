#include "header.h"
#include "balance.hpp"
#include "area.hpp"

#include <opencv/cv.h>

#include <vector>
#include <algorithm>
#include <iostream>

using namespace cv;
using namespace std;

typedef cv::Mat_<cv::Vec3b> Image;

static void get_matrix(Image & mat, unsigned char* buf, mirosot_vision_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    mat = img;
}
static void copy_to(const Image& mat, unsigned char* buf) {
    if (buf) {
        memcpy(buf, mat.ptr(), 3 * mat.size().width * mat.size().height);
    }
}

void init_config(mirosot_vision_config* config) {
    config->meanshift_radius = 5;
    config->meanshift_threshold = 40;
    
    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->debug_balance = NULL;
    config->debug_prescreen = NULL;
    config->debug_meanshift = NULL;
}

bool is_black(Vec3b c){
    return c[2]<120;
}
bool is_blue(Vec3b c){
    return !is_black(c) && c[0]>90 && c[0]<110 && c[1]*c[2]>128*128;
}


Image img_hsv;// TODO: make this reentrant
robot_data find_teams(mirosot_vision_config* config) {
    cv::Mat_<cv::Vec3b> img;
    get_matrix(img, config->image, config);
    
    white_balance(&img, config);
    copy_to(img, config->debug_balance);
    
    cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!
    
    Area area(*config);
    
    area.setImage(img);
    area.precompute(is_blue, img_hsv);

    area.meanShift();
    vector<PixelSet> areas = area.getSets();
    cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!!!

    if (config->debug_prescreen) {
        cout<<"areas: "<<areas.size()<<endl;
        Image img_prescreen(img.clone());
        img_prescreen*=0.5;
        for (int x=0;x<img.size().width;x++)
            for (int y=0;y<img.size().height;y++) {
            	int area_ind = area.area_map.get(x, y);
                if (area_ind)
                    img_prescreen(y, x) = Vec3b((area_ind%3)*255/2, ((area_ind/3)%3)*255/2, 255);
                else if (area.isIn(x, y))
                    img_prescreen(y, x) = Vec3b(255, 0, 0);
            }
            copy_to(img_prescreen, config->debug_prescreen);
    
    }
    
    
    copy_to(img, config->debug_meanshift);
    
    return robot_data();
}
