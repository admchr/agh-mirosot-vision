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

const double DEBUG_DIM = 0.5;

static void get_matrix(Image & mat, unsigned char* buf, mirosot_vision_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    if (config->linearize)
    	linearize(img);// 1ms

    mat = img;
}

static void copy_to(const Image& mat, unsigned char* buf, mirosot_vision_config* config) {
    if (buf) {
    	Image mat2(mat.clone());
    	if (config->linearize)
    		delinearize(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
    }
}

void init_config(mirosot_vision_config* config) {
    config->px_per_cm = 16/7.5;

    config->meanshift_radius = 4;
    config->meanshift_threshold = 30;

    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->mask_points = NULL;
    config->mask_points_len = 0;

    config->black_cutoff = 60;
    config->blue_min = 85;
    config->blue_max = 115;
    config->yellow_min = 10;
    config->yellow_max = 40;
    config->minimum_saturation = 100;
    config->white_cutoff = 110;

    config->linearize = 0;

    config->debug_balance = NULL;
    config->debug_prescreen = NULL;
    config->debug_meanshift = NULL;
    config->debug_patches = NULL;
    config->debug_robots = NULL;

    config->state = NULL;
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
    copy_to(img_white, config->debug_balance, config);
}

static void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, VisionState* vs, mirosot_vision_config *config)
{
    Image img_prescreen(img.clone());
    img_prescreen *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
        	if (vs->converter.get(img(y, x))[2] < config->black_cutoff)
        		img_prescreen(y, x) = Vec3b(0, 0, 0);
            PatchType *patch = area.precompute_map.get(x, y);
        	if(patch)
                img_prescreen(y, x) = patch->color;
        	if (vs->converter.get(img(y, x))[2] > config->white_cutoff)
        		img_prescreen(y, x) = Vec3b(255, 255, 255);

        }

    copy_to(img_prescreen, config->debug_prescreen, config);
}

static void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, mirosot_vision_config *config)
{
    Image img_patches(img.clone());
    img_patches *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);
            if (area_ind) {
                int n = (intptr_t)area_ind;
                assert(area_ind->type);
                img_patches(y, x) = Vec3b((n%3)*255/3, (n/3%3)*255/3, 255);
            }
        }

    copy_to(img_patches, config->debug_patches, config);
}

static void debugTeam(Image& img, const team_data& team) {

    for (int i=0; i<team.team_len; i++) {
        robot_data robot = team.team[i];
        image_pos p = robot.position;
        double angle = robot.angle;
        for (int i=0;i<20;i++) {
            int x = p.x + cos(angle)*i;
            int y = p.y + sin(angle)*i;
            if (x < 0 || y < 0 || x >= img.cols || y >= img.rows)
                break;
            img(y, x) = Vec3b(0, 0, 255);
        }
    }
}

static void debugRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const vision_data& robots, mirosot_vision_config *config)
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
    copy_to(img_robots, config->debug_robots, config);
}

inline bool is_lil_blue(mirosot_vision_config* config, Vec3b c){
    return
    		c[2] > config->black_cutoff &&
    		c[0] > config->blue_min &&
    		c[0] < config->blue_max &&
    		c[1] > config->minimum_saturation;
}
inline bool is_lil_yellow(mirosot_vision_config* config, Vec3b c){
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
    mirosot_vision_config* config;
    PatchType* operator()(Vec3b c){
        if(is_lil_blue(config, c))
            return blue;
        if(is_lil_yellow(config, c))
            return yellow;
        return 0;
    }
};

VisionState* newVisionState(mirosot_vision_config* config) {
	vector<Point> poly;
	for (int i=0; i<config->mask_points_len; i++) {
		image_pos pos = config->mask_points[i];
		poly.push_back(Point(pos.x, pos.y));
	}
	VisionState* state = new VisionState();

	state->mask.init(poly, Size(config->width, config->height));

	return state;
}

vision_data find_teams(mirosot_vision_config* config) {
    vision_data robots;

	if (!config->state) {
		config->state = newVisionState(config);
	}
    VisionState* state = static_cast<VisionState*>(config->state);

    Image img;
    get_matrix(img, config->image, config);//1ms
    Image img_hsv(img.clone());//<1ms
    
    white_balance(&img, config);//6ms
    if (config->debug_balance) {
    	debugWhite(img, config);
    }
    state->mask.apply(img);//5ms
    state->converter.convert(img, img_hsv);//1ms
    PatchFinder area(*config);

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
    if (config->debug_prescreen) {
        debugPrescreen(img, area, state, config);
    }

    area.getSets();//25ms


    blue.fillTeam(&robots.blue_team);
    yellow.fillTeam(&robots.yellow_team);


    if (config->debug_patches) {
        debugPatches(img, area, config);
    }
    if (config->debug_robots) {
        debugRobots(img, area, robots, config);
    }
    
    copy_to(img, config->debug_meanshift, config);
    /**/
    return robots;
}
