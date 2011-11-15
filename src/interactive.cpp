#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>

#include "amv.h"
#include "common.hpp"

using namespace std;

amv_config config;
amv_debug_info debug;

int dbg_i;
int debug_count = 5;

char** images;
int img_i = 0;
int image_count;
cv::Mat_<cv::Vec3b> img0;


int main(int argc, char** argv)
{
    images = argv + 2;
    image_count = argc - 2;
    img0 = cv::imread(images[img_i]);
    const char* trackbar_window = "Controls";
    const char* trackbar_window2 = "Controls2";
    const char* image_window = "Image";

    cv::namedWindow( trackbar_window, CV_WINDOW_NORMAL );
    cv::namedWindow( trackbar_window2, CV_WINDOW_NORMAL );
    cv::namedWindow( image_window, CV_WINDOW_NORMAL );

    amv_config_init(&config);
    load_config(&config, argv[1]);
    config.width = img0.cols;
    config.height = img0.rows;

    amv_debug_init(&debug);

    cv::Mat_<cv::Vec3b> dbg_img(img0.clone());

    cv::createTrackbar( "num", trackbar_window, &config.team_size, 255, 0, 0 );
    cv::createTrackbar( "black", trackbar_window, &config.black_cutoff, 255, 0, 0 );
    cv::createTrackbar( "white", trackbar_window, &config.white_cutoff, 255, 0, 0 );
    cv::createTrackbar( "y1", trackbar_window, &config.yellow.hue_min, 255, 0, 0 );
    cv::createTrackbar( "y2", trackbar_window, &config.yellow.hue_max, 255, 0, 0 );
    cv::createTrackbar( "b1", trackbar_window, &config.blue.hue_min, 255, 0, 0 );
    cv::createTrackbar( "b2", trackbar_window, &config.blue.hue_max, 255, 0, 0 );
    cv::createTrackbar( "o1", trackbar_window, &config.orange.hue_min, 255, 0, 0 );
    cv::createTrackbar( "o2", trackbar_window, &config.orange.hue_max, 255, 0, 0 );
    cv::createTrackbar( "sat", trackbar_window, &config.minimum_saturation, 255, 0, 0 );
    cv::createTrackbar( "dst", trackbar_window, &config.same_color_distance, 255, 0, 0 );
    cv::createTrackbar( "msr", trackbar_window, &config.meanshift_radius, 255, 0, 0 );
    cv::createTrackbar( "mst", trackbar_window, &config.meanshift_threshold, 255, 0, 0 );
    cv::createTrackbar( "fms", trackbar_window2, &debug.full_meanshift_debug, 1, 0, 0 );
    cv::createTrackbar( "lms", trackbar_window2, &debug.linear_meanshift, 1, 0, 0 );
    cv::createTrackbar( "mms", trackbar_window2, &debug.multiple_meanshift, 4, 0, 0 );
    cv::createTrackbar( "bh", trackbar_window2, config.team_hue, 180, 0, 0 );
    cv::createTrackbar( "gh", trackbar_window2, config.team_hue+1, 180, 0, 0 );
    cv::createTrackbar( "rh", trackbar_window2, config.team_hue+2, 180, 0, 0 );

    while(true) {
        amv_state state;
        cv::Mat_<cv::Vec3b> img(img0.clone());
        amv_state_new(state, config);

        if (dbg_i == 0)
            debug.debug_balance = dbg_img.ptr();
        else
            debug.debug_balance = 0;

        if (dbg_i == 1)
            debug.debug_meanshift = dbg_img.ptr();
        else
            debug.debug_meanshift = 0;

        if (dbg_i == 2)
            debug.debug_prescreen = dbg_img.ptr();
        else
            debug.debug_prescreen = 0;

        if (dbg_i == 3)
            debug.debug_patches = dbg_img.ptr();
        else
            debug.debug_patches = 0;

        if (dbg_i == 4)
            debug.debug_robots = dbg_img.ptr();
        else
            debug.debug_robots = 0;

        amv_find_teams(img.ptr(), &state, &debug);

        cv::imshow(image_window, dbg_img);
        int k = (signed char)cv::waitKey( 500 );

        //cout<<"k="<<k+0<<endl;
        if( k == 113 || k == 27) {//'q'||ESC
            break;
        }
        if ( k == 32) {//' '
            dbg_i++;
            dbg_i%=debug_count;
        }
        if ( k == 8) {//backspace
            dbg_i += debug_count;
            dbg_i--;
            dbg_i%=debug_count;
        }
        if (k == 91 || k == 93) {// '[' ']'
            img_i+=image_count;
            if (k==93)
                img_i++;
            else
                img_i--;
            img_i%=image_count;
            img0 = cv::imread(images[img_i]);
        }
    }

    return 0;
}
