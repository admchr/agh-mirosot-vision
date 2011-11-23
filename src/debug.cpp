#include "debug.hpp"
#include "linearize.hpp"
#include "visionstate.hpp"

using namespace cv;

const double DEBUG_DIM = 0.5;

void amv_debug_init(amv_debug_info* debug) {
    debug->debug_balance = NULL;
    debug->debug_prescreen = NULL;
    debug->debug_meanshift = NULL;
    debug->debug_patches = NULL;
    debug->debug_robots = NULL;
    debug->debug_results = NULL;

    debug->full_meanshift_debug = 1;
    debug->linear_meanshift = 1;
    debug->multiple_meanshift = 1;
}

static void copy_to(const Image& mat, unsigned char* buf, amv_config* config) {
    if (buf) {
        Image mat2(mat.clone());
        if (config->linearize)
            delinearize(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
    }
}

void paintPoint(Image img, Point p, Vec3b color) {
    if(p.x < 0 || p.y < 0 || p.x >= img.cols || p.y >= img.rows)
        return;

    img(p) = color;
}

void debugLine(amv_image_pos p, double angle, Image & img, int len, Vec3b color)
{
    for(int i = 0;i < len;i++){
        int x = p.x + cos(angle) * i;
        int y = p.y + sin(angle) * i;
        if(x < 0 || y < 0 || x >= img.cols || y >= img.rows)
            break;

        img(y, x) = color;
    }
}

void debugImageWhite(cv::Mat_<cv::Vec3b> & img, amv_config *config, amv_debug_info* debug)
{
    if (!debug->debug_balance)
        return;
    Image img_white(img.clone());
    for(int i = 0;i < config->white_points_len;i++) {
        amv_image_pos pos = config->white_points[i];
        img_white(pos.y, pos.x) = cv::Vec3b(0, 0, 255);
        for (int i=0; i<4; i++)
            debugLine(pos, i*M_PI/2, img_white, 5, Vec3b(0, 0, 255));
    }
    copy_to(img_white, debug->debug_balance, config);
}

void debugImagePrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_state *state, amv_debug_info* debug)
{
    if (!debug->debug_prescreen)
        return;
    Image img_prescreen(img.clone());
    img_prescreen *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            if (hsvconverter.get(img(y, x))[2] < state->config->black_cutoff)
                img_prescreen(y, x) = Vec3b(0, 0, 0);
            PatchType *patch = area.precompute_map.get(x, y);
            if(patch)
                img_prescreen(y, x) = patch->color;
            if (hsvconverter.get(img(y, x))[2] > state->config->white_cutoff)
                img_prescreen(y, x) = Vec3b(255, 255, 255);
        }

    copy_to(img_prescreen, debug->debug_prescreen, state->config);
}

void debugImagePatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_config *config, amv_debug_info* debug)
{
    if (!debug->debug_patches)
        return;
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

void debugTeam(Image& img, const amv_team_data& team, Vec3b primary) {
    const int SIDE = 9;

    Vec3b instanceColors[3];
    instanceColors[2] = Vec3b(0, 0, 255);
    instanceColors[1] = Vec3b(0, 255, 0);
    instanceColors[0] = Vec3b(255, 0, 0);


    for (int i=0; i<team.team_len; i++) {
        amv_robot_data robot = team.team[i];
        amv_image_pos p = robot.position;
        amv_image_pos tmp = p;
        double angle = robot.angle;
        double front_x = cos(angle)*SIDE;
        double front_y = sin(angle)*SIDE;

        double right_side_x = cos(angle+M_PI*0.5)*SIDE;
        double right_side_y = sin(angle+M_PI*0.5)*SIDE;
        debugLine(p, angle, img, 20, primary);

        //right
        tmp.x=p.x+front_x+right_side_x;
        tmp.y=p.y+front_y+right_side_y;
        debugLine(tmp, angle+M_PI, img, 2*SIDE, primary);
        // front
        tmp.x=p.x+front_x-right_side_x;
        tmp.y=p.y+front_y-right_side_y;
        debugLine(tmp, angle+M_PI/2, img, 2*SIDE, primary);

        // left
        tmp.x=p.x-front_x-right_side_x;
        tmp.y=p.y-front_y-right_side_y;
        debugLine(tmp, angle, img, 2*SIDE, instanceColors[robot.color[1]]);

        // back
        tmp.x=p.x-front_x+right_side_x;
        tmp.y=p.y-front_y+right_side_y;
        debugLine(tmp, angle+M_PI*3/2, img, 2*SIDE, instanceColors[robot.color[0]]);
    }
}

void debugSecondaryPatches(Image& img, vector<Patch*> patches) {
    int out[2];
    for (unsigned int i=0; i<patches.size(); i++) {
        patches[i]->getSecondaryPatches(out, &img);
    }
}

void debugImageRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const amv_vision_data& robots, amv_config* config, amv_debug_info *debug, std::vector<Patch*> blue, std::vector<Patch*> yellow)
{
    if (!debug->debug_robots)
        return;
    Image img_robots(img.clone());

    img_robots *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);

            if (area_ind && (area_ind->isRobot || area_ind->isBall)) {
                img_robots(y, x) = area_ind->type->color;
            }
        }

    debugSecondaryPatches(img_robots, blue);
    debugSecondaryPatches(img_robots, yellow);

    copy_to(img_robots, debug->debug_robots, config);
}

void debugImageMeanshift(amv_debug_info *debug, Image img, amv_config *config)
{
    if (!debug->debug_meanshift)
        return;
    if(debug->full_meanshift_debug)
        for (int i=0; i<debug->multiple_meanshift; i++) {
            if (debug->linear_meanshift)
                meanShiftFilteringProportional(img, config->meanshift_radius, config->meanshift_threshold);
            else
                meanShiftFiltering(img, config->meanshift_radius, config->meanshift_threshold);
        }
    copy_to(img, debug->debug_meanshift, config);
}

void debugImageResults(Image& img, amv_vision_data* data, amv_config* config, amv_debug_info* debug) {
    if (!debug->debug_results)
        return;
    Image img_robots(img.clone());

    //img_robots *= DEBUG_DIM;
    debugTeam(img_robots, data->blue_team, Vec3b(255, 0, 0));
    debugTeam(img_robots, data->yellow_team, Vec3b(0, 255, 255));

    for (int i=0; i<4; i++)
        debugLine(data->ball_pos, M_PI*0.5*i, img_robots, 10, Vec3b(0, 0, 255));

    copy_to(img_robots, debug->debug_results, config);

}
