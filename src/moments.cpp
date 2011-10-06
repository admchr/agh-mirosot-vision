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
    return (sum_xx - sum_x*sum_x/count)/(count - 1.0);
}

double PatchMoments::getYVariance() {
    return (sum_yy - sum_y*sum_y/count)/(count - 1.0);
}

double PatchMoments::getCovariance() {
    return (sum_xy - sum_x*sum_y/count)/(count - 1.0);
}

double PatchMoments::getRegressionSlope() {
    // Deming regression
    double var_x = getXVariance();
    double var_y = getYVariance();
    double var_xy = getCovariance();
    double b1 = (var_y - var_x + sqrt((var_y - var_x) * (var_y - var_x) + 4 * var_xy * var_xy)) / 2 / var_xy;
    return b1;
}

double PatchMoments::getRegressionPosition() {

	double b1 = getRegressionSlope();
    double b0 = sum_y*1.0/count - b1*sum_x/count;
    return b0;
}

double PatchMoments::getAngle() {
    // Deming regression
    double b1 = getRegressionSlope();
    return atan(b1);
}
