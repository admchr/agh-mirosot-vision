#include "amv.h"
#include "balance.hpp"
#include "area.hpp"
#include "defs.hpp"
#include "visionstate.hpp"
#include "linearize.hpp"
#include "debug.hpp"
#include "util.hpp"

#include <opencv/cv.h>

#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <stdint.h>

using namespace cv;
using namespace std;

static void get_matrix(Image & mat, unsigned char* buf, amv_config* config) {
    cv::Mat img_tmp(cv::Size(config->image_width, config->image_height), CV_8UC3, buf);
    Image img(img_tmp);
    if (config->linearize)
    	linearize(img);// 1ms

    mat = img;
}

void amv_config_init(amv_config* config) {
    config->px_per_cm = 16/7.5;

    config->meanshift_radius = 0;
    config->meanshift_threshold = 36;//=0

    config->same_color_distance = 250;

    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->mask_points = NULL;
    config->mask_points_len = 0;

    config->blue.color.hue_min = 85;
    config->blue.color.hue_max = 110;
    config->yellow.color.hue_min = 20;
    config->yellow.color.hue_max = 40;
    config->white_is_yellow = 1;
    config->orange.hue_min = 0;
    config->orange.hue_max = 20;

    config->yellow.team_size = 5;
    config->blue.team_size = 5;

    config->blue.secondary_colors[0].hue_min = 115;
    config->blue.secondary_colors[0].hue_max = 144;
    config->blue.secondary_colors[1].hue_min = 44;
    config->blue.secondary_colors[1].hue_max = 87;
    config->blue.secondary_colors[2].hue_min = 164;
    config->blue.secondary_colors[2].hue_max = 20;
    for (int i=0; i<AMV_MAX_SECONDARY_COLORS;  i++)
        config->yellow.secondary_colors[i] = config->blue.secondary_colors[i];

    config->blue.robot_info[0].back_color = 0;
    config->blue.robot_info[0].front_color = 1;
    config->blue.robot_info[1].back_color = 0;
    config->blue.robot_info[1].front_color = 2;
    config->blue.robot_info[2].back_color = 1;
    config->blue.robot_info[2].front_color = 0;
    config->blue.robot_info[3].back_color = 1;
    config->blue.robot_info[3].front_color = 2;
    config->blue.robot_info[4].back_color = 2;
    config->blue.robot_info[4].front_color = 1;

    for (int i=0; i<config->blue.team_size;  i++)
        config->yellow.robot_info[i] = config->blue.robot_info[i];

    config->blue.home_team = 1;
    config->yellow.home_team = 1;

    config->minimum_saturation = 100;
    config->white_cutoff = 125;
    config->black_cutoff = 70;

    config->white_is_yellow = 1;
    config->linearize = 0;

    config->transform.field_top_left.x = 103;
    config->transform.field_top_left.y = 4;
    config->transform.field_bottom_right.x = 600;
    config->transform.field_bottom_right.y = 400;
    config->transform.output_scale.x = 10;
    config->transform.output_scale.y = 10;
}


void amv_state_new(amv_state* state, amv_config* config) {
    vector<Point> poly;
    for (int i=0; i<config->mask_points_len; i++) {
        amv_image_pos pos = config->mask_points[i];
        poly.push_back(Point(pos.x, pos.y));
    }
    state->config = config;
    state->state = (void*) new VisionState();
    ((VisionState*) state->state)->mask.init(poly, Size(config->image_width, config->image_height));
}

void amv_state_free(amv_state* state) {
    VisionState* vs = (VisionState*)(state->state);
    state->state = 0;
    if (vs)
        delete vs;
}

struct Precompute {
    PatchType* blue;
    PatchType* yellow;
    PatchType* orange;
    amv_config* config;
    PatchType* operator()(Vec3b c){
        if(is_patch(config, &config->blue.color, c, false))
            return blue;
        if(is_patch(config, &config->yellow.color, c, true))
            return yellow;
        if(is_patch(config, &config->orange, c, false))
            return orange;
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
    get_matrix(img, image, config);
    Image img_hsv(img.clone());
    white_balance(&img, config);
    debugImageWhite(img, config, debug);

    ((VisionState*) state->state)->mask.apply(img);
    hsvconverter.convert(img, img_hsv);
    PatchFinder area;

    area.setImages(state, img, img_hsv);
    PatchType blue(&area, config->blue.color, Vec3b(255, 0, 0), config, false);
    PatchType yellow(&area, config->yellow.color, Vec3b(0, 255, 255), config, false);
    PatchType orange(&area, config->orange, Vec3b(0, 128, 255), config, false);
    Precompute precompute;
    precompute.config = config;
    precompute.blue = &blue;
    precompute.yellow = &yellow;
    precompute.orange = &orange;
    area.precompute(precompute);
    debugImagePrescreen(img, area, state, debug);
    area.getSets();

    vector<Robot> yellowTeam, blueTeam;
    yellowTeam = getTeam(&yellow, &config->yellow);
    blueTeam = getTeam(&blue, &config->blue);
    Patch* ball = orange.getBall();

    fillTeam(yellowTeam, &robots.yellow_team);
    fillTeam(blueTeam, &robots.blue_team);
    fillBall(ball, &robots);

    debugImagePatches(img, area, config, debug);
    debugImageRobots(img, area, config, debug, blueTeam, yellowTeam);
    debugImageResults(img, &robots, config, debug);
    debugImageMeanshift(debug, img, config);

    transformTeam(&robots.blue_team, config->transform);
    transformTeam(&robots.yellow_team, config->transform);
    transformPosition(&robots.ball_pos, config->transform);
//*/

    return robots;
}
