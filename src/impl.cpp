#include "header.h"
#include "balance.hpp"
#include "area.hpp"
#include "defs.hpp"
#include "visionstate.hpp"
#include "linearize.hpp"

#include <opencv/cv.h>

#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <stdint.h>


using namespace cv;
using namespace std;


static void get_matrix(Image & mat, unsigned char* buf, mirosot_vision_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    linearize(img);// 8ms
    mat = img;
}

static void copy_to(const Image& mat, unsigned char* buf) {
    if (buf) {
    	Image mat2(mat.clone());
    	delinearize(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
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
    config->debug_robots = NULL;

    config->state = new VisionState();
}

void free_config(mirosot_vision_config* config) {
    delete static_cast<VisionState*>(config->state);
}



static void debugWhite(cv::Mat_<cv::Vec3b> & img, mirosot_vision_config *config)
{
    Image img_white(img.clone());
    for(int i = 0;i < config->white_points_len;i++) {
    	image_pos pos = config->white_points[i];
    	img_white(pos.y, pos.x) = cv::Vec3b(0, 0, 255);
    }
    copy_to(img_white, config->debug_balance);
}

static void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_prescreen(img.clone());
    img_prescreen *= 0.1;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            PatchType *patch = area.precompute_map.get(x, y);
        	if(patch)
                img_prescreen(y, x) = patch->color;

        }

    copy_to(img_prescreen, config->debug_prescreen);
}

static void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_patches(img.clone());
    img_patches *= 0.1;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind) {
                int n = (intptr_t)area_ind;
                assert(area_ind->type);
                img_patches(y, x) = Vec3b((n%3)*255/3, (n/3%3)*255/3, 255);
            }
        }

    copy_to(img_patches, config->debug_patches);
}

static void debugRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_robots(img.clone());
    img_robots *= 0.1;
    set<Patch*> patches;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind && area_ind->isLegal()) {
                img_robots(y, x) = area_ind->type->color;
                patches.insert(area_ind);
            }
        }

    for (set<Patch*>::iterator it = patches.begin(); it!=patches.end(); it++){
        Patch* pa = *it;
        Point p = pa->getMean();
        double angle = pa->getAngle();
        //cout<<angle<<" "<<sqrt(pa->moments.getXVariance())<<" "<<sqrt(pa->moments.getYVariance())<<" "<<pa->moments.getCovariance()<<endl;
        angle+=M_PI*0.25;
        for (int i=0;i<20;i++) {
            int x = p.x + cos(angle)*i;
            int y = p.y + sin(angle)*i;
            if (x < 0 || y < 0 || x >= img.cols || y >= img.rows)
                break;
            img_robots(y, x) = Vec3b(0, 0, 255);
        }
        img_robots(p) = Vec3b(0, 0, 255);
    }
    copy_to(img_robots, config->debug_robots);
}

static bool is_black(Vec3b c){
    return c[2]<40;
}

struct Precompute {
    PatchType* blue;
    PatchType* yellow;
    PatchType* orange;

    PatchType* operator()(Vec3b c){
        if(!is_black(c) && c[0]>85 && c[0]<115 && c[1]*c[2]>128*128/4)
            return blue;
        if(!is_black(c) && c[0]>15 && c[0]<40)
            return yellow;
        return 0;
    }
};

bool is_lil_blue(Vec3b c){
    return c[2]>70 && c[0]>85 && c[0]<115 && c[1]*c[2]>128*128/8 && c[1]>55;
}
bool is_lil_yellow(Vec3b c){
    return (c[2]>70 && c[0]>15 && c[0]<40) || c[2]>120;// && c[1]*c[2]>128*128/2;
}

robot_data find_teams(mirosot_vision_config* config) {
    Image img;
    get_matrix(img, config->image, config);//9ms!!!
    Image img_hsv(img.clone());//1ms
    
    white_balance(&img, config);//6ms
    if (config->debug_balance) {
    	debugWhite(img, config);
    }
    //cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!
    VisionState* state = static_cast<VisionState*>(config->state);
    state->converter.convert(img, img_hsv);//1ms
    PatchFinder area(*config);
    
    area.setImages(img, img_hsv);
    PatchType blue(&area, is_lil_blue, Vec3b(255, 0, 0));
    PatchType yellow(&area, is_lil_yellow, Vec3b(0, 255, 255));
    PatchType orange(&area, is_lil_blue, Vec3b(0, 0, 255));
    Precompute precompute;
    precompute.blue = &blue;
    precompute.yellow = &yellow;
    precompute.orange = &orange;
    area.precompute(precompute);//<1ms
    if (config->debug_prescreen) {
        debugPrescreen(img, area, config);
    }

    area.getSets();//10ms

    robot_data robots;

    blue.fillLegal(robots.team1, &robots.team1_len);
    yellow.fillLegal(robots.team2, &robots.team2_len);


    if (config->debug_patches) {
        debugPatches(img, area, config);
    }
    if (config->debug_robots) {
        debugRobots(img, area, config);
    }
    
    copy_to(img, config->debug_meanshift);
    
    return robots;
}
