#include "amv.h"
#include "balance.hpp"
#include "patch.hpp"
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
    	from_sRGB_to_RGB(img);// 1ms

    mat = img;
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

amv_vision_data findTeams(unsigned char* image, amv_state* state, amv_debug_info* debug) {
    amv_debug_info tmp_debug;
    amv_debug_init(&tmp_debug);
    if (!debug) debug = &tmp_debug;

    amv_vision_data robots;
    amv_config* config = state->config;

    Image img;
    get_matrix(img, image, config);
    Image img_hsv(img.clone());
    WhitePoints whitePoints = getWhitePoints(img, config);
    white_balance(img, whitePoints);
    debugImageWhite(img, config, debug);

    ((VisionState*) state->state)->mask.apply(img);
    hsvconverter.fromBGRToHSL(img, img_hsv);
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
