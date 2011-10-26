#include "amv.h"
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

const double DEBUG_DIM = 0.5;

static void get_matrix(Image & mat, unsigned char* buf, amv_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    if (config->linearize)
    	linearize(img);// 1ms

    mat = img;
}

static void copy_to(const Image& mat, unsigned char* buf, amv_config* config) {
    if (buf) {
    	Image mat2(mat.clone());
    	if (config->linearize)
    		delinearize(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
    }
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

static void debugWhite(cv::Mat_<cv::Vec3b> & img, amv_config *config, amv_debug_info* debug)
{
    Image img_white(img.clone());
    for(int i = 0;i < config->white_points_len;i++) {
    	amv_image_pos pos = config->white_points[i];
    	img_white(pos.y, pos.x) = cv::Vec3b(0, 0, 255);
    }
    copy_to(img_white, debug->debug_balance, config);
}

static void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_state *state, amv_debug_info* debug)
{
    Image img_prescreen(img.clone());
    img_prescreen *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
        	if (((VisionState*) state->state)->converter.get(img(y, x))[2] < state->config->black_cutoff)
        		img_prescreen(y, x) = Vec3b(0, 0, 0);
            PatchType *patch = area.precompute_map.get(x, y);
        	if(patch)
                img_prescreen(y, x) = patch->color;
        	if (((VisionState*) state->state)->converter.get(img(y, x))[2] > state->config->white_cutoff)
        		img_prescreen(y, x) = Vec3b(255, 255, 255);
        }

    copy_to(img_prescreen, debug->debug_prescreen, state->config);
}

static void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_config *config, amv_debug_info* debug)
{
    Image img_patches(img.clone());
    img_patches *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind) {
                int n = (intptr_t)area_ind;
                assert(area_ind->type);
                int c = 7;
                img_patches(y, x) = Vec3b((n%c)*255/(c-1), (n/c%c)*255/(c-1), 255);
            }
        }

    copy_to(img_patches, debug->debug_patches, config);
}

void debugLine(amv_image_pos p, double angle, Image & img, int len)
{
    for(int i = 0;i < len;i++){
        int x = p.x + cos(angle) * i;
        int y = p.y + sin(angle) * i;
        if(x < 0 || y < 0 || x >= img.cols || y >= img.rows)
            break;

        img(y, x) = Vec3b(0, 0, 255);
    }
}

static void debugTeam(Image& img, const amv_team_data& team) {
	const int SIDE = 9;
    for (int i=0; i<team.team_len; i++) {
        amv_robot_data robot = team.team[i];
        amv_image_pos p = robot.position;
        amv_image_pos tmp = p;
        double angle = robot.angle;
        double front_x = cos(angle)*SIDE;
        double front_y = sin(angle)*SIDE;

        double side_x = cos(angle+M_PI*0.5)*SIDE;
        double side_y = sin(angle+M_PI*0.5)*SIDE;
        debugLine(p, angle, img, 20);

        tmp.x=p.x+front_x+side_x;
        tmp.y=p.y+front_y+side_y;
        debugLine(tmp, angle+M_PI, img, 2*SIDE);
        tmp.x=p.x+front_x-side_x;
        tmp.y=p.y+front_y-side_y;
        debugLine(tmp, angle+M_PI/2, img, 2*SIDE);
        tmp.x=p.x-front_x-side_x;
        tmp.y=p.y-front_y-side_y;
        debugLine(tmp, angle, img, 2*SIDE);
        tmp.x=p.x-front_x+side_x;
        tmp.y=p.y-front_y+side_y;
        debugLine(tmp, angle+M_PI*3/2, img, 2*SIDE);

    }
}

static void debugRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const amv_vision_data& robots, amv_config* config, amv_debug_info *debug)
{
    Image img_robots(img.clone());
    img_robots *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind && area_ind->isLegal()) {
                img_robots(y, x) = area_ind->type->color;
            }
        }
    debugTeam(img_robots, robots.blue_team);
    debugTeam(img_robots, robots.yellow_team);
    copy_to(img_robots, debug->debug_robots, config);
}

inline bool is_lil_blue(amv_config* config, Vec3b c){
    return
    		c[2] > config->black_cutoff &&
    		c[0] > config->blue_min &&
    		c[0] < config->blue_max &&
    		c[1] > config->minimum_saturation;
}
inline bool is_lil_yellow(amv_config* config, Vec3b c){
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
        if(is_lil_blue(config, c))
            return blue;
        if(is_lil_yellow(config, c))
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
    ((VisionState*) state->state)->converter.convert(img, img_hsv);//1ms
    PatchFinder area(state);

    area.setImages(img, img_hsv);
    PatchType blue(&area, is_lil_blue, Vec3b(255, 0, 0), config);
    PatchType yellow(&area, is_lil_yellow, Vec3b(0, 255, 255), config);
    PatchType orange(&area, is_lil_blue, Vec3b(0, 0, 255), config);
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
        if (debug->full_meanshift_debug)
            meanShiftFiltering(img, config->meanshift_radius, config->meanshift_threshold);
        copy_to(img, debug->debug_meanshift, config);
    }
//*/
    robots.ball_pos.x = robots.ball_pos.y = 0;

    return robots;
}
