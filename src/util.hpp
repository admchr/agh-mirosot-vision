#ifndef UTIL_H
#define UTIL_H

#include <opencv/cv.h>
#include "amv.h"

inline static bool is_robot(amv_config* config, amv_team_info* team, cv::Vec3b c){
    return
            (c[2] > config->black_cutoff &&
            c[0] > team->hue_min &&
            c[0] < team->hue_max &&
            c[1] > config->minimum_saturation) ||
            (
                c[2] > config->white_cutoff &&
                team->captures_white
            );
}

double positive_value_certainty(double min, double max, double actual);

#endif
