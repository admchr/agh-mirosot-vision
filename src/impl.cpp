#include "header.h"

#include <opencv/cv.h>

#include <iostream>

static void get_matrix(cv::Mat_<cv::Vec3b> & mat, unsigned char * buf, mirosot_vision_conf*cnf) {
    cv::Mat/*_<cv::Vec3b>*/ img_tmp(cv::Size(cnf->width, cnf->height), CV_8UC3, buf);
    cv::Mat_<cv::Vec3b> img(img_tmp);
    mat = img;
}
static void copy_to(const cv::Mat_<cv::Vec3b> & mat, unsigned char * buf) {
    if (buf) {
        memcpy(buf, mat.ptr(), 3 * mat.size().width * mat.size().height);
    }
}

void init_config(mirosot_vision_conf*cnf) {
    cnf->meanshift_radius = 5;
    cnf->meanshift_threshold = 20;
    cnf->debug_balance = NULL;
    cnf->debug_meanshift = NULL;
}

CV_IMPL void
meanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
    double sp0, double sr, int max_level,
    CvTermCriteria termcrit );

robot_data find_teams(mirosot_vision_conf*cnf) {
    cv::Mat_<cv::Vec3b> img;
    get_matrix(img, cnf->image, cnf);
    CvMat img_mat = (CvMat)img;
    
    
    
    // in place meanshift - gives different results
    // than normal procedure
    meanShiftFiltering(
        &img_mat,
        &img_mat,
        cnf->meanshift_radius, //spatial
        cnf->meanshift_threshold, //channel
        0,
        cv::TermCriteria(CV_TERMCRIT_ITER, 5, 1.0)
    );
    img = cv::Mat(&img_mat);
    
    copy_to(img, cnf->debug_meanshift);
}

void white_balance() {
}