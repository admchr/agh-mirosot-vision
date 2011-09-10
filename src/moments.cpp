#include "moments.hpp"

using namespace cv;

PatchMoments::PatchMoments() {
    count = sum_x = sum_y = 0;
}

void PatchMoments::add(Point p) {
    count++;
    sum_x+=p.x;
    sum_y+=p.y;
}
Moments PatchMoments::getMoments() {
    return Moments(count, sum_x, sum_y, 0, 0,
            0, 0, 0, 0, 0);
}
Point PatchMoments::getMean() {
    Moments m = getMoments();
    return Point(sum_x/count, sum_y/count);
}
int PatchMoments::getCount() {
    return count;
}
