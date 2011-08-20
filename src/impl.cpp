#include "header.h"

#include <opencv/cv.h>

robot_data find_teams(mirosot_vision_conf*cnf) {
    cv::Mat_<cv::Vec3b> img(cv::Size(cnf->width, cnf->height), *cnf->image);
    
    
}
