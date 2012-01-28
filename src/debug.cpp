#include "debug.hpp"
#include "linearize.hpp"
#include "visionstate.hpp"
#include <algorithm>

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
        paintPoint(img, Point(x, y), color);
    }
}
void debugLine(amv_point p, double angle, Image & img, int len, Vec3b color) {
    amv_image_pos pos;
    pos.x = p.x;
    pos.y = p.y;
    debugLine(pos, angle, img, len, color);
}

void debugLine(Point p, double angle, Image & img, int len, Vec3b color) {
    amv_image_pos pos;
    pos.x = p.x;
    pos.y = p.y;
    debugLine(pos, angle, img, len, color);
}

void debugLine(amv_image_pos p1, amv_image_pos p2, Image & img, Vec3b color)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    if (abs(dx) > abs(dy)) {
        if (p1.x >= p2.x) {
            dx = -dx;
            dy = -dy;
            std::swap(p1, p2);
        }
        for(int i = 0;i < dx;i++){
            int x = p1.x + i;
            int y = p1.y + i*1.0*dy/dx;
            paintPoint(img, Point(x, y), color);
        }
    } else {
        if (p1.y >= p2.y) {
            std::swap(p1, p2);
            dy = -dy;
            dx = -dx;
        }
        for(int i = 0;i < dy;i++){
            int x = p1.x + i*1.0*dx/dy;
            int y = p1.y + i;
            paintPoint(img, Point(x, y), color);
        }
    }
}

void debugImageWhite(Image & img, amv_config *config, amv_debug_info* debug)
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

void debugImagePrescreen(Image & img, PatchFinder & area, amv_state *state, amv_debug_info* debug)
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

void debugImagePatches(Image & img, PatchFinder & area, amv_config *config, amv_debug_info* debug)
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

void debugTeam(Image& img, amv_config *config, amv_team_info& info, const amv_team_data& team, Vec3b primary) {
    double frame_side = 7.5*config->px_per_cm/2;

    Vec3b instanceColors[3];
    instanceColors[2] = Vec3b(0, 0, 255);
    instanceColors[1] = Vec3b(0, 255, 0);
    instanceColors[0] = Vec3b(255, 0, 255);


    for (int i=0; i<team.team_len; i++) {
        amv_robot_data robot = team.team[i];
        amv_image_pos p;
        p.x = robot.position.x;
        p.y = robot.position.y;
        amv_image_pos tmp = p;

        if (info.home_team) {
            double angle = robot.angle;
            double front_x = cos(angle)*frame_side;
            double front_y = sin(angle)*frame_side;

            double right_side_x = cos(angle+M_PI*0.5)*frame_side;
            double right_side_y = sin(angle+M_PI*0.5)*frame_side;
            debugLine(p, angle, img, 2*frame_side, primary);

            //right
            tmp.x=p.x+front_x+right_side_x;
            tmp.y=p.y+front_y+right_side_y;
            debugLine(tmp, angle+M_PI, img, 2*frame_side, primary);
            // front
            tmp.x=p.x+front_x-right_side_x;
            tmp.y=p.y+front_y-right_side_y;
            debugLine(tmp, angle+M_PI/2, img, 2*frame_side, primary);

            amv_robot_info secondary = info.robot_info[robot.identity];
            // left
            tmp.x=p.x-front_x-right_side_x;
            tmp.y=p.y-front_y-right_side_y;
            debugLine(tmp, angle, img, 2*frame_side, instanceColors[secondary.front_color]);

            // back
            tmp.x=p.x-front_x+right_side_x;
            tmp.y=p.y-front_y+right_side_y;
            debugLine(tmp, angle+M_PI*3/2, img, 2*frame_side, instanceColors[secondary.back_color]);
        } else {
            amv_image_pos p1, p2, p3, p4;
            p1.x = p.x - frame_side;
            p1.y = p.y - frame_side;
            p2.x = p.x - frame_side;
            p2.y = p.y + frame_side;
            p3.x = p.x + frame_side;
            p3.y = p.y + frame_side;
            p4.x = p.x + frame_side;
            p4.y = p.y - frame_side;
            debugLine(p1, p2, img, primary);
            debugLine(p2, p3, img, primary);
            debugLine(p3, p4, img, primary);
            debugLine(p4, p1, img, primary);
        }
    }
}

void debugSecondaryPatches(Image& img, amv_team_info* team, vector<Robot> patches) {
    for (unsigned int i=0; i<patches.size(); i++) {
        Robot& p = patches[i];
        getSecondaryPatches(patches[i].teamPatch, team, &img);
    }
}

void debugImageRobots(Image& img, PatchFinder & area, amv_config* config, amv_debug_info *debug, std::vector<Robot> blue, std::vector<Robot> yellow)
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

    debugSecondaryPatches(img_robots, &config->blue, blue);
    debugSecondaryPatches(img_robots, &config->yellow, yellow);

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
    debugTeam(img_robots, config, config->blue, data->blue_team, Vec3b(255, 0, 0));
    debugTeam(img_robots, config, config->yellow, data->yellow_team, Vec3b(0, 255, 255));

    for (int i=0; i<4; i++)
        debugLine(data->ball_pos, M_PI*0.5*i, img_robots, 10, Vec3b(0, 0, 255));

    amv_image_pos tl, tr, bl, br;
    tl = config->transform.field_top_left;
    br = config->transform.field_bottom_right;
    tr.x = br.x;
    tr.y = tl.y;
    bl.x = tl.x;
    bl.y = br.y;

    debugLine(tr, tl, img_robots, Vec3b(255, 255, 255));
    debugLine(tl, bl, img_robots, Vec3b(255, 255, 255));
    debugLine(bl, br, img_robots, Vec3b(255, 255, 255));
    debugLine(br, tr, img_robots, Vec3b(255, 255, 255));
    copy_to(img_robots, debug->debug_results, config);

}
