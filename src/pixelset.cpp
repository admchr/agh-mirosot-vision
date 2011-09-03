#include "pixelset.hpp"


void PixelSet::add(cv::Point p, cv::Vec3b color){
    count++;
}
int PixelSet::getCount() {
    return count;
}

bool PixelSet::meetsRobotCriteria(mirosot_vision_config* config) {
    double min_size = config->px_per_cm * 3.5 * 0.75;
    double max_size = config->px_per_cm * 7.5 * 1.25;
    int min_area = min_size*min_size;
    int max_area = max_size*max_size;

    return getCount() <= max_area && getCount() >= min_area;

}
