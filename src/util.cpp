#include "util.hpp"

#include "hsvconverter.hpp"
#include <cmath>

using namespace cv;

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

Vec3b getMeanColor(amv_color_info c)
{
    int mean_vec = c.hue_max - c.hue_min;
    if(mean_vec < 0)
        mean_vec += 180;

    int mean = (c.hue_min + mean_vec / 2 + 180) % 180;
    Vec3b paint = hsvconverter.getBGR(Vec3b(mean, 255, 255));
    if(c.hue_min == c.hue_max)
        paint = Vec3b(255, 255, 255);

    return paint;
}

void transformPosition(amv_point* pos, amv_transform_info tr) {
    amv_point size;
    size.x = tr.field_bottom_right.x - tr.field_top_left.x;
    size.y = tr.field_bottom_right.y - tr.field_top_left.y;

    // subtract top left
    pos->x = pos->x - tr.field_top_left.x;
    pos->y = pos->y - tr.field_top_left.y;

    // rescale to [0,1]
    pos->x /= size.x;
    pos->y /= size.y;

    // output scale
    pos->x *= tr.output_scale.x;
    pos->y *= tr.output_scale.y;

}

Point toPoint(amv_image_pos pos) {
    return Point(pos.x, pos.y);
}
Point toPoint(amv_point pos) {
    return Point(pos.x, pos.y);
}
