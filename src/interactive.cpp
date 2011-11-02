#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>

#include "amv.h"
#include "common.hpp"

using namespace std;

amv_config config;
amv_debug_info debug;
int g_switch_value = 0;
int colorInt = 0;

// Trackbar/switch callback
void switch_callback( int position, void* ){
    if( position == 0 ){
         colorInt = 0;
    }else{
         colorInt = 1;
    }

}

int dbg_i;

int main(int argc, char** argv)
{
    cv::Mat_<cv::Vec3b> img0 = cv::imread(argv[1]);
    const char* name = "Demo Window";
    int radius = 30;
    int thickness = 2;
    int connectivity = 8;
    CvScalar green = CV_RGB(0,250,0);
    CvScalar orange = CV_RGB(250,150,0);

    CvPoint pt2 = cvPoint(405,195);

    cv::namedWindow( name, 1 );


    amv_config_init(&config);
    load_config(&config, argv[2]);
    config.width = img0.cols;
    config.height = img0.rows;


    cv::Mat_<cv::Vec3b> dbg_img(img0.clone());

    cv::createTrackbar( "black", name, &config.black_cutoff, 255, 0, 0 );
    cv::createTrackbar( "white", name, &config.white_cutoff, 255, 0, 0 );
    cv::createTrackbar( "y1", name, &config.yellow_min, 255, 0, 0 );
    cv::createTrackbar( "y2", name, &config.yellow_max, 255, 0, 0 );
    cv::createTrackbar( "b1", name, &config.blue_min, 255, 0, 0 );
    cv::createTrackbar( "b2", name, &config.blue_max, 255, 0, 0 );
    cv::createTrackbar( "sat", name, &config.minimum_saturation, 255, 0, 0 );
    cv::createTrackbar( "msr", name, &config.meanshift_radius, 255, 0, 0 );
    cv::createTrackbar( "mst", name, &config.meanshift_threshold, 255, 0, 0 );

    while(true) {
        amv_state state;
        cv::Mat_<cv::Vec3b> img(img0.clone());
        amv_state_new(state, config);

        amv_debug_init(&debug);
        if (dbg_i == 0)
            debug.debug_balance = dbg_img.ptr();
        if (dbg_i == 1)
            debug.debug_prescreen = dbg_img.ptr();
        if (dbg_i == 2)
            debug.debug_meanshift = dbg_img.ptr();
        if (dbg_i == 3)
            debug.debug_patches = dbg_img.ptr();
        if (dbg_i == 4)
            debug.debug_robots = dbg_img.ptr();

        amv_find_teams(img.ptr(), &state, &debug);

        cv::imshow(name, dbg_img);

        int k = (signed char)cv::waitKey( 500 );

        //cout<<"k="<<k+0<<endl;

        if( k == 113 || k == 27) {//'q'||ESC
            break;
        }
        if ( k == 32) {//' '
            dbg_i++;
            dbg_i%=5;
        }
    }

    return 0;
}
