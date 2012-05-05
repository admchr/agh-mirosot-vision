#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>

#include "amv.h"
#include "common.hpp"

using namespace std;

amv_config config;
amv_debug_info debug;

int debug_image;
int debug_image_count = 7;

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
    config.image_width = img0.cols;
    config.image_height = img0.rows;

    amv_debug_init(&debug);

    cv::Mat_<cv::Vec3b> dbg_img(img0.clone());

    cv::createTrackbar( "numb", trackbar_window, &config.blue.team_size, 255, 0, 0 );
    cv::createTrackbar( "numy", trackbar_window, &config.yellow.team_size, 255, 0, 0 );
    cv::createTrackbar( "black", trackbar_window, &config.black_cutoff, 255, 0, 0 );
    cv::createTrackbar( "white", trackbar_window, &config.white_cutoff, 255, 0, 0 );
    cv::createTrackbar( "y1", trackbar_window, &config.yellow.color.hue_min, 255, 0, 0 );
    cv::createTrackbar( "y2", trackbar_window, &config.yellow.color.hue_max, 255, 0, 0 );
    cv::createTrackbar( "b1", trackbar_window, &config.blue.color.hue_min, 255, 0, 0 );
    cv::createTrackbar( "b2", trackbar_window, &config.blue.color.hue_max, 255, 0, 0 );
    cv::createTrackbar( "o1", trackbar_window, &config.orange.hue_min, 255, 0, 0 );
    cv::createTrackbar( "o2", trackbar_window, &config.orange.hue_max, 255, 0, 0 );
    cv::createTrackbar( "sat", trackbar_window, &config.minimum_saturation, 255, 0, 0 );
    cv::createTrackbar( "dst", trackbar_window2, &config.same_color_distance, 255, 0, 0 );
    cv::createTrackbar( "msr", trackbar_window2, &config.meanshift_radius, 255, 0, 0 );
    cv::createTrackbar( "mst", trackbar_window2, &config.meanshift_threshold, 255, 0, 0 );
    cv::createTrackbar( "fms", trackbar_window2, &debug.full_meanshift_debug, 1, 0, 0 );
    cv::createTrackbar( "lms", trackbar_window2, &debug.linear_meanshift, 1, 0, 0 );
    cv::createTrackbar( "mms", trackbar_window2, &debug.multiple_meanshift, 4, 0, 0 );
    cv::createTrackbar( "bh[", trackbar_window2, &config.yellow.secondary_colors[0].hue_min, 180, 0, 0 );
    cv::createTrackbar( "bh]", trackbar_window2, &config.yellow.secondary_colors[0].hue_max, 180, 0, 0 );
    cv::createTrackbar( "gh[", trackbar_window2, &config.yellow.secondary_colors[1].hue_min, 180, 0, 0 );
    cv::createTrackbar( "gh]", trackbar_window2, &config.yellow.secondary_colors[1].hue_max, 180, 0, 0 );
    cv::createTrackbar( "rh[", trackbar_window2, &config.yellow.secondary_colors[2].hue_min, 180, 0, 0 );
    cv::createTrackbar( "rh]", trackbar_window2, &config.yellow.secondary_colors[2].hue_max, 180, 0, 0 );

    while(true) {
        for (int i=0; i<3; i++) {
            config.blue.secondary_colors[i] = config.yellow.secondary_colors[i];
        }
        amv_state state;
        cv::Mat_<cv::Vec3b> img(img0.clone());
        amv_state_new(&state, &config);

        if (debug_image == 0)
            dbg_img = img0.clone();

        if (debug_image == 1)
            debug.debug_balance = dbg_img.ptr();
        else
            debug.debug_balance = 0;

        if (debug_image == 2)
            debug.debug_meanshift = dbg_img.ptr();
        else
            debug.debug_meanshift = 0;

        if (debug_image == 3)
            debug.debug_prescreen = dbg_img.ptr();
        else
            debug.debug_prescreen = 0;

        if (debug_image == 4)
            debug.debug_patches = dbg_img.ptr();
        else
            debug.debug_patches = 0;

        if (debug_image == 5)
            debug.debug_robots = dbg_img.ptr();
        else
            debug.debug_robots = 0;

        if (debug_image == 6)
            debug.debug_results = dbg_img.ptr();
        else
            debug.debug_results = 0;

        amv_find_teams(img.ptr(), &state, &debug);

        cv::imshow(image_window, dbg_img);
        int key = (signed char)cv::waitKey( 500 );

        //cout<<"key="<<k+0<<endl;
        if( key == 113 || key == 27) {//'q'||ESC
            break;
        }
        if ( key == 32) {//' '
            debug_image++;
            debug_image%=debug_image_count;
        }
        if ( key == 8) {//backspace
            debug_image += debug_image_count;
            debug_image--;
            debug_image%=debug_image_count;
        }
        if (key == 91 || key == 93) {// '[' ']'
            img_i+=image_count;
            if (key==93)
                img_i++;
            else
                img_i--;
            img_i%=image_count;
            img0 = cv::imread(images[img_i]);
            cout << "loaded: " << images[img_i] <<endl;
        }
        if (key == 46) {// "."
            config.angle_method = (config.angle_method + 1)%AMV_MAX_ANGLE_METHOD;
        }
        if (key == 44) {// ","
            config.angle_method = (config.angle_method - 1 + AMV_MAX_ANGLE_METHOD)%AMV_MAX_ANGLE_METHOD;
        }
    }

    return 0;
}
