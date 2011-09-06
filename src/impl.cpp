#include "header.h"
#include "balance.hpp"
#include "area.hpp"
#include "defs.hpp"

#include <opencv/cv.h>

#include <vector>
#include <algorithm>
#include <iostream>
#include <stdint.h>

using namespace cv;
using namespace std;


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
    config->px_per_cm = 16/7.5;

    config->meanshift_radius = 4;
    config->meanshift_threshold = 50;
    
    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->debug_balance = NULL;
    config->debug_prescreen = NULL;
    config->debug_meanshift = NULL;
    config->debug_patches = NULL;
}

bool is_black(Vec3b c){
    return c[2]<120;
}
bool is_blue(Vec3b c){
    return !is_black(c) && c[0]>90 && c[0]<110 && c[1]*c[2]>128*128;
}


static void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_patches(img.clone());
    img_patches *= 0.3;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind && area_ind->isLegal()) {
            	int n = (intptr_t)area_ind;
                img_patches(y, x) = Vec3b((n%3)*255/3, (n/3%3)*255/3, 255);
            }

        }

    copy_to(img_patches, config->debug_patches);
}

static void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_prescreen(img.clone());
    img_prescreen *= 0.3;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            if(area.precompute_map.get(x, y))
                img_prescreen(y, x) = Vec3b(255, 0, 0);

        }

    copy_to(img_prescreen, config->debug_prescreen);
}

robot_data find_teams(mirosot_vision_config* config) {
    Image img_hsv;
    cv::Mat_<cv::Vec3b> img;
    get_matrix(img, config->image, config);
    
    white_balance(&img, config);
    copy_to(img, config->debug_balance);
    
    cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!
    
    PatchFinder area(*config);
    
    area.setImage(img);
    PatchType type(&area);
    type.precompute(is_blue, img_hsv);
    if (config->debug_prescreen) {
        debugPrescreen(img, area, config);
    }

    area.getSets();

    if (config->debug_patches) {
        debugPatches(img, area, config);
    }
    
    copy_to(img, config->debug_meanshift);
    
    return robot_data();
}
