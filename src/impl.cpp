#include "header.h"
#include "balance.hpp"
#include <opencv/cv.h>

#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

typedef cv::Mat_<cv::Vec3b> Image;

static void get_matrix(Image & mat, unsigned char* buf, mirosot_vision_config* config) {
    cv::Mat img_tmp(cv::Size(config->width, config->height), CV_8UC3, buf);
    Image img(img_tmp);
    mat = img;
}
static void copy_to(const Image& mat, unsigned char* buf) {
    if (buf) {
        memcpy(buf, mat.ptr(), 3 * mat.size().width * mat.size().height);
    }
}

void init_config(mirosot_vision_config* config) {
    config->meanshift_radius = 5;
    config->meanshift_threshold = 20;
    
    config->white_points = NULL;
    config->white_points_len = 0;
    
    config->debug_balance = NULL;
    config->debug_meanshift = NULL;
}


CV_IMPL void
meanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
    double sp0, double sr, int max_level,
    CvTermCriteria termcrit );

robot_data find_teams(mirosot_vision_config* config) {
    cv::Mat_<cv::Vec3b> img;
    get_matrix(img, config->image, config);
    CvMat img_mat = (CvMat)img;
    
    white_balance(&img, config);
    copy_to(img, config->debug_balance);
    
    // in place meanshift - gives different results
    // than normal procedure
    /*meanShiftFiltering(
        &img_mat,
        &img_mat,
        config->meanshift_radius, // position radius
        config->meanshift_threshold, // value radius
        0,
        cv::TermCriteria(CV_TERMCRIT_ITER, 5, 1.0)
    );*/
    img = cv::Mat(&img_mat);
    
    copy_to(img, config->debug_meanshift);
    
    return robot_data();
}
