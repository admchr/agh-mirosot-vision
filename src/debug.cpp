#include "debug.hpp"
#include "linearize.hpp"
#include "visionstate.hpp"

using namespace cv;

const double DEBUG_DIM = 0.5;

static void copy_to(const Image& mat, unsigned char* buf, amv_config* config) {
    if (buf) {
        Image mat2(mat.clone());
        if (config->linearize)
            delinearize(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
    }
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

void debugWhite(cv::Mat_<cv::Vec3b> & img, amv_config *config, amv_debug_info* debug)
{
    Image img_white(img.clone());
    for(int i = 0;i < config->white_points_len;i++) {
        amv_image_pos pos = config->white_points[i];
        img_white(pos.y, pos.x) = cv::Vec3b(0, 0, 255);
        for (int i=0; i<4; i++)
            debugLine(pos, i*M_PI/2, img_white, 5);
    }
    copy_to(img_white, debug->debug_balance, config);
}

void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_state *state, amv_debug_info* debug)
{
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

void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_config *config, amv_debug_info* debug)
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

void debugTeam(Image& img, const amv_team_data& team) {
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

void debugRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const amv_vision_data& robots, amv_config* config, amv_debug_info *debug)
{
    Image img_robots(img.clone());
    img_robots *= DEBUG_DIM;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            Patch* area_ind = area.area_map.get(x, y);

            if (area_ind && (area_ind->isRobot || area_ind->isBall)) {
                img_robots(y, x) = area_ind->type->color;
            }
        }
    debugTeam(img_robots, robots.blue_team);
    debugTeam(img_robots, robots.yellow_team);

    for (int i=0; i<4; i++)
        debugLine(robots.ball_pos, M_PI*0.5*i, img_robots, 10);

    copy_to(img_robots, debug->debug_robots, config);
}

void debugMeanshift(amv_debug_info *debug, Image img, amv_config *config)
{
    if(debug->full_meanshift_debug)
        meanShiftFiltering(img, config->meanshift_radius, config->meanshift_threshold);

    copy_to(img, debug->debug_meanshift, config);
}
