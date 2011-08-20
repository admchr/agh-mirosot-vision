#include "header.h"

#include <opencv/cv.h>

#include <iostream>


CV_IMPL void
meanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
                         double sp0, double sr, int max_level,
                         CvTermCriteria termcrit );
robot_data find_teams(mirosot_vision_conf*cnf) {
    cv::Mat/*_<cv::Vec3b>*/ img_tmp(cv::Size(cnf->width, cnf->height), CV_8UC3, cnf->image);
    cv::Mat_<cv::Vec3b> img(img_tmp);
//     pyrMeanShiftFiltering( InputArray src, OutputArray dst,
//     double sp, double sr, int maxLevel=1,
//      TermCriteria termcrit=TermCriteria(
//                                                 TermCriteria::MAX_ITER+TermCriteria::EPS,5,1) );
    CvMat img_mat = (CvMat)img;
    meanShiftFiltering(
        &img_mat,
        &img_mat,
        cnf->meanshift_radius,//spatial
        cnf->meanshift_threshold,//channel
        0,
        cv::TermCriteria(CV_TERMCRIT_ITER, 5, 1.0)
    );
    img = cv::Mat(&img_mat);
}
void init_config(mirosot_vision_conf*cnf) {
    cnf->meanshift_radius = 5;
    cnf->meanshift_threshold = 20;
}
