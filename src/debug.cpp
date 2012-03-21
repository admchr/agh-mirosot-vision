#include "debug.hpp"
#include "linearize.hpp"
#include "util.hpp"
#include "visionstate.hpp"
#include "font.hpp"
#include <algorithm>
#include <sstream>

using namespace cv;
using namespace std;


const double DEBUG_DIMMING = 0.5;

/** Copies image data to the buffer */
static void copy_to(const Image& mat, unsigned char* buf, amv_config* config) {
    if (buf) {
        Image mat2(mat.clone());
        if (config->linearize)
            from_RGB_to_sRGB(mat2);
        memcpy(buf, mat2.ptr(), 3 * mat2.size().width * mat2.size().height);
    }
}

void drawPoint(Image img, Point p, Vec3b color) {
    if(p.x < 0 || p.y < 0 || p.x >= img.cols || p.y >= img.rows)
        return;

    img(p) = color;
}

void drawLine(Image& img, Point p1, Point p2, Vec3b color)
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
            drawPoint(img, Point(x, y), color);
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
            drawPoint(img, Point(x, y), color);
        }
    }
}

void drawLine(Image& img, Point p1, double angle, double length, Vec3b color) {
    Point p2(p1.x + length*cos(angle),  p1.y + length*sin(angle));
    return drawLine(img, p1, p2, color);
}

void drawCross(Image& img, Point center, int armLength, Vec3b color) {
    drawLine(img, center, Point(center.x, center.y + armLength), color);
    drawLine(img, center, Point(center.x, center.y - armLength), color);
    drawLine(img, center, Point(center.x + armLength, center.y), color);
    drawLine(img, center, Point(center.x - armLength, center.y), color);
}

void drawChar(Image& img, cv::Point origin, unsigned char character, cv::Vec3b color) {
    unsigned char *glyph = font_data[character];
    int size = FONT_SIZE;

    for (int dx = 0; dx < size; dx++)
        for (int dy = 0; dy < size; dy++) {
            Point p = origin + Point(dx, dy - size);
            if ((glyph[dy] >> (size - dx)) & 1)
                drawPoint(img, p, color);
        }
}

void drawText(Image& img, cv::Point origin, std::string text, cv::Vec3b color) {
    for (unsigned int i=0; i<text.length(); i++) {
        drawChar(img, origin + Point(i*FONT_SIZE, 0), text[i], color);
    }
}

void drawBorderText(Image& img, cv::Point origin, std::string text, cv::Vec3b color, cv::Vec3b border) {
    drawText(img, origin + Point(1, 0), text, border);
    drawText(img, origin + Point(0, 1), text, border);
    drawText(img, origin + Point(-1, 0), text, border);
    drawText(img, origin + Point(0, -1), text, border);

    drawText(img, origin, text, color);
}

void debugImageWhite(Image & img, amv_config *config, amv_debug_info* debug)
{
    if (!debug->debug_balance)
        return;
    Image img_white(img.clone());
    for(int i = 0;i < config->white_points_len;i++) {
        Point pos = toPoint(config->white_points[i]);
        img_white(pos.y, pos.x) = cv::Vec3b(0, 0, 255);
        drawCross(img_white, pos, 5, Vec3b(0, 0, 255));
    }
    copy_to(img_white, debug->debug_balance, config);
}

void debugImagePrescreen(Image & img, PatchFinder & area, amv_state *state, amv_debug_info* debug)
{
    if (!debug->debug_prescreen)
        return;
    Image img_prescreen(img.clone());
    img_prescreen *= DEBUG_DIMMING;
    for(int x = 0;x < img.size().width;x++)
        for(int y = 0;y < img.size().height;y++){
            if (hsvconverter.fromBGRToHSL(img(y, x))[2] < state->config->black_cutoff)
                img_prescreen(y, x) = Vec3b(0, 0, 0);
            PatchType *patch = area.precompute_map.get(x, y);
            if(patch)
                img_prescreen(y, x) = patch->color;
            if (hsvconverter.fromBGRToHSL(img(y, x))[2] > state->config->white_cutoff)
                img_prescreen(y, x) = Vec3b(255, 255, 255);
        }

    copy_to(img_prescreen, debug->debug_prescreen, state->config);
}

void debugImagePatches(Image & img, PatchFinder & area, amv_config *config, amv_debug_info* debug)
{
    if (!debug->debug_patches)
        return;
    Image img_patches(img.clone());
    img_patches *= DEBUG_DIMMING;
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


    for (int i=0; i<team.team_len; i++) {
        amv_robot_data robot = team.team[i];
        Point p = toPoint(robot.position);

        if (info.home_team) {
            double angle = robot.angle;
            Point front(cos(angle)*frame_side, sin(angle)*frame_side);
            Point right(cos(angle+M_PI*0.5)*frame_side, sin(angle+M_PI*0.5)*frame_side);

            drawLine(img, p, p + front + front, primary);

            Point fr = p + front + right;
            Point fl = p + front - right;
            Point br = p - front + right;
            Point bl = p - front - right;

            amv_robot_info secondary = info.robot_info[robot.identity];
            drawLine(img, fr, fl, primary);
            drawLine(img, br, bl, getMeanColor(info.secondary_colors[secondary.back_color]));
            drawLine(img, fl, bl, getMeanColor(info.secondary_colors[secondary.front_color]));
            drawLine(img, fr, br, primary);

            stringstream str;
            str << robot.identity;
            drawBorderText(img, p + Point(0, -frame_side*2), str.str(), Vec3b(255, 255, 255), Vec3b(0, 0, 0));
        } else {
            Point p1, p2, p3, p4;
            p1.x = p.x - frame_side;
            p1.y = p.y - frame_side;
            p2.x = p.x - frame_side;
            p2.y = p.y + frame_side;
            p3.x = p.x + frame_side;
            p3.y = p.y + frame_side;
            p4.x = p.x + frame_side;
            p4.y = p.y - frame_side;
            drawLine(img, p1, p2, primary);
            drawLine(img, p2, p3, primary);
            drawLine(img, p3, p4, primary);
            drawLine(img, p4, p1, primary);
        }
    }
}

void debugSecondaryPatches(Image& img, amv_team_info* team, vector<Robot> patches) {
    for (unsigned int i=0; i<patches.size(); i++) {
        Robot& p = patches[i];
        p.teamPatch->getAngleFitness(p.teamPatch->getAngle(), &img);
        getSecondaryPatches(patches[i].teamPatch, team, &img);
    }
}

void debugImageRobots(Image& img, PatchFinder & area, amv_config* config, amv_debug_info *debug, std::vector<Robot> blue, std::vector<Robot> yellow)
{
    if (!debug->debug_robots)
        return;
    Image img_robots(img.clone());

    img_robots *= DEBUG_DIMMING;
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

    drawCross(img_robots, toPoint(data->ball_pos), 10, Vec3b(0, 0, 255));

    Point tl, tr, bl, br;
    tl = toPoint(config->transform.field_top_left);
    br = toPoint(config->transform.field_bottom_right);
    tr.x = br.x;
    tr.y = tl.y;
    bl.x = tl.x;
    bl.y = br.y;

    drawLine(img_robots, tr, tl, Vec3b(255, 255, 255));
    drawLine(img_robots, tl, bl, Vec3b(255, 255, 255));
    drawLine(img_robots, bl, br, Vec3b(255, 255, 255));
    drawLine(img_robots, br, tr, Vec3b(255, 255, 255));
    copy_to(img_robots, debug->debug_results, config);

}
