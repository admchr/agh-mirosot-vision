#ifndef AMV_DEBUG_HPP
#define AMV_DEBUG_HPP

#include "amv.h"
#include "defs.hpp"
#include "patch.hpp"
#include "robot.hpp"

#include <vector>
#include <string>

void drawPoint(Image img, cv::Point p, cv::Vec3b color);
void drawLine(Image& img, cv::Point p1, cv::Point p2, cv::Vec3b color);
void drawLine(Image& img, cv::Point p1, double angle, double length, cv::Vec3b color);
void drawCross(Image& img, cv::Point center, int armLength, cv::Vec3b color);
void drawText(Image& img, cv::Point origin, std::string text, cv::Vec3b color);
void drawBorderText(Image& img, cv::Point origin, std::string text, cv::Vec3b color, cv::Vec3b border);

void debugImageWhite(Image& img, amv_config *config, amv_debug_info* debug);
void debugImagePrescreen(Image & img, PatchFinder & area, amv_state *state, amv_debug_info* debug);
void debugImagePatches(Image & img, PatchFinder & area, amv_config *config, amv_debug_info* debug);
void debugSecondaryPatches(Image& img, amv_team_info* team, std::vector<Robot> patches);
void debugTeam(Image& img, const amv_team_data& team, cv::Vec3b primary = cv::Vec3b(255, 255, 255));
void debugImageRobots(Image& img, PatchFinder & area, amv_config* config, amv_debug_info *debug, std::vector<Robot> blue, std::vector<Robot> yellow);
void debugImageResults(Image & img, amv_vision_data* robots, amv_config* config, amv_debug_info *debug);
void debugImageMeanshift(amv_debug_info *debug, Image img, amv_config *config);

#endif
