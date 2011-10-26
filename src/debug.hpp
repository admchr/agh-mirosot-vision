#ifndef DEBUG_H
#define DEBUG_H

#include "amv.h"
#include "defs.hpp"
#include "area.hpp"

void debugLine(amv_image_pos p, double angle, Image & img, int len);
void debugWhite(cv::Mat_<cv::Vec3b> & img, amv_config *config, amv_debug_info* debug);
void debugPrescreen(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_state *state, amv_debug_info* debug);
void debugPatches(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, amv_config *config, amv_debug_info* debug);
void debugTeam(Image& img, const amv_team_data& team);
void debugRobots(cv::Mat_<cv::Vec3b> & img, PatchFinder & area, const amv_vision_data& robots, amv_config* config, amv_debug_info *debug);
void debugMeanshift(amv_debug_info *debug, Image img, amv_config *config);

#endif
