
#include "mask.hpp"

bool rightOf( cv::Point point, cv::Point line1, cv::Point line2) {
    int x1 = line1.x;
    int y1 = line1.y;
    int x2 = line2.x;
    int y2 = line2.y;
    int x = point.x;
    int y = point.y;
    double x_pos = x1 + ((x2-x1)*1.0/(y2-y1)*(y-y1));
    return x > x_pos;
}
