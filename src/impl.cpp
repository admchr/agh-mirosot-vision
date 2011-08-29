#include "header.h"
#include "balance.hpp"
#include "area.hpp"

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
    config->debug_prescreen = NULL;
    config->debug_meanshift = NULL;
}

bool is_black(Vec3b c){
    return c[2]<120;
}
bool is_blue(Vec3b c){
    return !is_black(c) && c[0]>90 && c[0]<110 && c[1]*c[2]>128*128;
}

CV_IMPL void
meanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
    double sp0, double sr, int max_level,
    CvTermCriteria termcrit );

Image img_hsv;// TODO: make this reentrant
robot_data find_teams(mirosot_vision_config* config) {
    cv::Mat_<cv::Vec3b> img;
    get_matrix(img, config->image, config);
    
    white_balance(&img, config);
    copy_to(img, config->debug_balance);
    
    cvtColor(img, img_hsv, CV_BGR2HSV);// SLOW!
    
    Area area;
    
    area.setImage(img_hsv);
    area.precompute(is_blue);
    if (config->debug_prescreen) {
        Image img_prescreen(img.clone());
        img_prescreen*=0.5;
        for (int x=0;x<img.size().width;x++)
            for (int y=0;y<img.size().height;y++) {
                if (area.isIn(x, y))
                    img_prescreen(y, x) = Vec3b(255, 0, 0);
                else if (area.tile_set.get(x/16, y/16)) 
                    img_prescreen(y, x) = Vec3b(255, 127, 127);
            }
            copy_to(img_prescreen, config->debug_prescreen);
    
    }
    
    CvMat img_mat = (CvMat)img;
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
