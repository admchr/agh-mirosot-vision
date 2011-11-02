#include "amv.h"
#include "balance.hpp"
#include "area.hpp"
#include "defs.hpp"
#include "visionstate.hpp"
#include "linearize.hpp"
#include "debug.hpp"

#include <opencv/cv.h>

#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <stdint.h>

using namespace cv;
using namespace std;

static void get_matrix(Image & mat, unsigned char* buf, amv_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    if (config->linearize)
    	linearize(img);// 1ms

    mat = img;
}


void amv_config_init(amv_config* config) {
    config->px_per_cm = 16/7.5;

    config->meanshift_radius = 3;
    config->meanshift_threshold = 40;

    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->mask_points = NULL;
    config->mask_points_len = 0;

    config->black_cutoff = 55;
    config->blue_min = 85;
    config->blue_max = 115;
    config->yellow_min = 10;
    config->yellow_max = 40;
    config->minimum_saturation = 100;
    config->white_cutoff = 110;

    config->linearize = 1;
}

void amv_debug_init(amv_debug_info* debug) {
    debug->debug_balance = NULL;
    debug->debug_prescreen = NULL;
    debug->debug_meanshift = NULL;
    debug->debug_patches = NULL;
    debug->debug_robots = NULL;

    debug->full_meanshift_debug = 1;
}

void amv_state_new(amv_state& state, amv_config& config) {
    vector<Point> poly;
    for (int i=0; i<config.mask_points_len; i++) {
        amv_image_pos pos = config.mask_points[i];
        poly.push_back(Point(pos.x, pos.y));
    }
    state.config = &config;
    state.state = (void*) new VisionState();
    ((VisionState*) state.state)->mask.init(poly, Size(config.width, config.height));
}

void amv_state_free(amv_state* state) {
    VisionState* vs = (VisionState*)(state->state);
    state->state = 0;
    if (vs)
        delete vs;
}

inline bool is_blue(amv_config* config, Vec3b c){
    return
    		c[2] > config->black_cutoff &&
    		c[0] > config->blue_min &&
    		c[0] < config->blue_max &&
    		c[1] > config->minimum_saturation;
}
inline bool is_yellow(amv_config* config, Vec3b c){
    return (
				c[2] > config->black_cutoff &&
				c[0] > config->yellow_min &&
				c[0] < config->yellow_max &&
				(c[1] > config->minimum_saturation || c[2] > config->white_cutoff)
    		) ||
    		(
    			c[2] > config->white_cutoff &&
                c[1] < 50
    		);// && c[1]*c[2]>128*128/2;
}

struct Precompute {
    PatchType* blue;
    PatchType* yellow;
    PatchType* orange;
    amv_config* config;
    PatchType* operator()(Vec3b c){
        if(is_blue(config, c))
            return blue;
        if(is_yellow(config, c))
            return yellow;
        return 0;
    }
};


amv_vision_data amv_find_teams(unsigned char* image, amv_state* state, amv_debug_info* debug) {
    amv_debug_info tmp_debug;
    amv_debug_init(&tmp_debug);
    if (!debug) debug = &tmp_debug;


    amv_vision_data robots;
    amv_config* config = state->config;

    Image img;
    get_matrix(img, image, config);//1ms
    Image img_hsv(img.clone());//<1ms
    
    white_balance(&img, config);//6ms
    if (debug->debug_balance) {
    	debugWhite(img, config, debug);
    }
    ((VisionState*) state->state)->mask.apply(img);//5ms
    hsvconverter.convert(img, img_hsv);//1ms
    PatchFinder area(state);

    area.setImages(img, img_hsv);
    PatchType blue(&area, is_blue, Vec3b(255, 0, 0), config);
    PatchType yellow(&area, is_yellow, Vec3b(0, 255, 255), config);
    PatchType orange(&area, is_blue, Vec3b(0, 0, 255), config);
    Precompute precompute;
    precompute.config = config;
    precompute.blue = &blue;
    precompute.yellow = &yellow;
    precompute.orange = &orange;
    area.precompute(precompute);//<1ms
    if (debug->debug_prescreen) {
        debugPrescreen(img, area, state, debug);
    }

    area.getSets();//25ms

    blue.fillTeam(&robots.blue_team);
    yellow.fillTeam(&robots.yellow_team);


    if (debug->debug_patches) {
        debugPatches(img, area, config, debug);
    }
    if (debug->debug_robots) {
        debugRobots(img, area, robots, config, debug);
    }
    if (debug->debug_meanshift) {
        debugMeanshift(debug, img, config);
    }
//*/
    robots.ball_pos.x = robots.ball_pos.y = 0;

    return robots;
}
