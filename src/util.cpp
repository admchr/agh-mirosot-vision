#include "util.hpp"

#include <cmath>

double logistic_cdf(double x) {
    return 1/(1+exp(-x));
}

double interval_certainty(double min, double max, double actual) {
    double scale = (max - min)/4;
    return logistic_cdf((actual - min)/scale) * logistic_cdf((max - actual)/scale);
}
double positive_interval_certainty(double min, double max, double actual) {
    return interval_certainty(log(min), log(max), log(actual));
}
double certainty_or(double p, double q) {
    return 1 - (1-p)*(1-q);
}
