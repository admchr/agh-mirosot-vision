#ifndef ROBOT_H
#define ROBOT_H

#include <opencv/cv.h>
#include "area.hpp"

class Patch;

struct Robot {
    Patch* teamPatch;
    cv::Point pos;
    double angle;
    double certainty;

    std::vector<double> identityCertainities;
    int identity;
};

std::vector<Robot> getTeam(PatchType* patchType, amv_team_info* team);
std::vector<double> getSecondaryPatches(Patch* patch, amv_team_info* team, Image* debug=0);
void fillTeam(std::vector<Robot> team, amv_team_data* data);
void fillBall(Patch* ball, amv_vision_data* data);
void transformTeam(amv_team_data* robots, amv_transform_info transform);


#endif
