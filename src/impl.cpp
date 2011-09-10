#include "header.h"
#include "balance.hpp"
#include "area.hpp"
#include "defs.hpp"
#include "visionstate.hpp"

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

    config->state = new VisionState();
}

void free_config(mirosot_vision_config* config) {
    delete static_cast<VisionState*>(config->state);
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

static bool is_black(Vec3b c){
    return c[2]<120;
}

struct Precompute {
    PatchType* blue;
    PatchType* yellow;
    PatchType* orange;

    PatchType* operator()(Vec3b c){
        if(!is_black(c) && c[0]>90 && c[0]<110 && c[1]*c[2]>128*128)
            return blue;
        if(!is_black(c) && c[0]>15 && c[0]<40)
            return yellow;
        return 0;
    }
};

bool is_lil_blue(Vec3b c){
    return c[2]>120 && c[0]>85 && c[0]<115 && c[1]*c[2]>128*128/4;
}
bool is_lil_yellow(Vec3b c){
    return c[2]>120 && c[0]>15 && c[0]<40;// && c[1]*c[2]>128*128/2;
}

robot_data find_teams(mirosot_vision_config* config) {
    Image img;
    get_matrix(img, config->image, config);
    Image img_hsv(img.clone());
    
    white_balance(&img, config);
    copy_to(img, config->debug_balance);
    
    //cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!
    VisionState* state = static_cast<VisionState*>(config->state);
    state->converter.convert(img, img_hsv);//!
    PatchFinder area(*config);
    
    area.setImages(img, img_hsv);
    PatchType blue(&area, is_lil_blue);
    PatchType yellow(&area, is_lil_yellow);
    PatchType orange(&area, is_lil_blue);
    Precompute precompute;
    precompute.blue = &blue;
    precompute.yellow = &yellow;
    precompute.orange = &orange;
    area.precompute(precompute);//12s
    if (config->debug_prescreen) {
        debugPrescreen(img, area, config);
    }

    area.getSets();//5s

    if (config->debug_patches) {
        debugPatches(img, area, config);
    }
    
    copy_to(img, config->debug_meanshift);
    
    return robot_data();
}
