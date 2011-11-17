#ifndef UTIL_H
#define UTIL_H

#include <opencv/cv.h>
#include "amv.h"

inline static bool is_robot(amv_config* config, amv_color_info* team, cv::Vec3b c){
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

double point_certainty(double expected, double actual);
double positive_point_certainty(double expected, double actual);
double interval_certainty(double min, double max, double actual);
double positive_interval_certainty(double min, double max, double actual);
double certainty_or(double p, double q);

int hue_distance(int a, int b);

#endif
