#ifndef DEBUG_H
#define DEBUG_H

#include "amv.h"
#include "defs.hpp"
#include "area.hpp"

void paintPoint(Image img, cv::Point p, cv::Vec3b color);

void debugLine(amv_image_pos p, double angle, Image & img, int len, cv::Vec3b color);
void debugImageWhite(cv::Mat_<cv::Vec3b> & img, amv_config *config, amv_debug_info* debug);
void debugImagePrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_state *state, amv_debug_info* debug);
void debugImagePatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_config *config, amv_debug_info* debug);
void debugSecondaryPatches(Image& img, std::vector<Patch*> patches);
void debugTeam(Image& img, const amv_team_data& team);
void debugImageRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const amv_vision_data& robots, amv_config* config, amv_debug_info *debug, std::vector<Patch*> blue, std::vector<Patch*> yellow);
void debugImageMeanshift(amv_debug_info *debug, Image img, amv_config *config);

#endif
