#ifndef UTIL_H
#define UTIL_H

#include <opencv/cv.h>
#include "amv.h"

inline static bool is_patch(amv_config* config, amv_color_info* color, cv::Vec3b hsv){
    return
            (hsv[2] > config->black_cutoff &&
            hsv[0] > color->hue_min &&
            hsv[0] < color->hue_max &&
            hsv[1] > config->minimum_saturation) ||
            (
                hsv[2] > config->white_cutoff &&
                color->captures_white
            );
}

double point_certainty(double expected, double actual);
double positive_point_certainty(double expected, double actual);
double interval_certainty(double min, double max, double actual);
double positive_interval_certainty(double min, double max, double actual);
double certainty_or(double p, double q);

int hue_distance(int a, int b);
bool in_hue(amv_color_info* color, int hue);

#endif
