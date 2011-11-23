#include "util.hpp"

#include <cmath>


double logistic_cdf(double x) {
    return 1/(1+exp(-x));
}

double interval_certainty(double min, double max, double actual) {
    double scale = (max - min)/4;
    return logistic_cdf((actual - min)/scale) * logistic_cdf((max - actual)/scale);
}

double point_certainty(double expected, double actual) {
    double d = expected - actual;
    return exp(-d*d);
}

double positive_point_certainty(double expected, double actual) {
    return point_certainty(log(expected), log(actual));
}

double positive_interval_certainty(double min, double max, double actual) {
    return interval_certainty(log(min), log(max), log(actual));
}
double certainty_or(double p, double q) {
    return 1 - (1-p)*(1-q);
}

int hue_distance(int a, int b) {
    int res = abs(a - b);
    return std::min(res, 180 - res);
}

bool in_hue(amv_color_info* color, int hue) {
    if (color->hue_min <= color->hue_max)
        return color->hue_min < hue && hue < color->hue_max;
    return color->hue_min < hue || hue < color->hue_max;
}
