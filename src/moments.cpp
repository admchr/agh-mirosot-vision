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

Point2d PatchMoments::getMean() {
    return Point2d(sum_x/count, sum_y/count);
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

double PatchMoments::getDeterminant() {
    return getXVariance()*getYVariance() - getCovariance()*getCovariance();
}

double PatchMoments::getRegressionSlope() {
    // Deming regression
    double var_x = getXVariance();
    double var_y = getYVariance();
    double var_xy = getCovariance();

    double tmp = sqrt((var_y - var_x) * (var_y - var_x) + 4 * var_xy * var_xy);

    double b1;
    if (var_y > var_x)
        b1 = (var_y - var_x + tmp) / 2 / var_xy;
    else
        b1 = 2 * var_xy / (var_x - var_y + tmp);
    if (b1 != b1) {// Microsoft C++ contains no isnan() function
        // return some predefined value, as we cannot compute it
        // this can occur for:
        // 1. empty set
        // 2. singleton point
        // 3. sets with no unique regression line
        return 0;
    }
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
