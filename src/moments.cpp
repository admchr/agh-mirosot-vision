#include "moments.hpp"

#include <cmath>

using namespace cv;

PatchMoments::PatchMoments() {
    count = sum_x = sum_y = sum_xx = sum_xy = sum_yy = 0;
}

void PatchMoments::add(Point p) {
    count++;
    sum_x+=p.x;
    sum_y+=p.y;
    sum_xx+=p.x*p.x;
    sum_xy+=p.x*p.y;
    sum_yy+=p.y*p.y;
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

double PatchMoments::getXVariance() {
    return (sum_x*sum_x - sum_xx)/(count - 1.0);
}

double PatchMoments::getYVariance() {
    return (sum_y*sum_y - sum_yy)/(count - 1.0);
}

double PatchMoments::getCovariance() {
    return (sum_x*sum_y - sum_xy)/(count - 1.0);
}

double PatchMoments::getAngle() {
    // Deming regression
    double var_x = getXVariance();
    double var_y = getYVariance();
    double var_xy = getCovariance();

    double b1 = (var_y - var_x + sqrt((var_y-var_x)*(var_y-var_x)+4*var_xy*var_xy))/2/var_xy;
    //double b0 = sum_y - b1*sum_x/count;
    // y = b0 + b1*x
    return atan(b1);
}
