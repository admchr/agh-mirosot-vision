#include "util.hpp"

#include <cmath>

double logistic_cdf(double x) {
    return 1/(1+exp(-x));
}

double positive_value_certainty(double min, double max, double actual) {
    double scale = (max - min)/4;
    return logistic_cdf((actual - min)/scale) * logistic_cdf((max - actual)/scale);
}
double certainty_or(double p, double q) {
    return 1 - (1-p)*(1-q);
}
