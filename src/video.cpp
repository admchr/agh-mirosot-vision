

#include "amv.h"
#include "common.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

using namespace cv;
using namespace std;

#define GET_CAP_PROP(prop) cout << #prop << "=" << cap.get(prop) << endl;

int main(int argc, char**argv) {
    if (argc != 3) {
        std::cerr<<"usage: "<<argv[0]<<" config video"<<std::endl;
        return 1;
    }

    VideoCapture cap;
    char* startptr = argv[2];
    char* endptr;
    long cap_id = strtol(startptr, &endptr, 10);
    if (endptr == startptr) {
        cout << "opening file: " << startptr << endl;
        cap.open(startptr);
    } else {
        cout << "opening camera: " << cap_id << endl;
        cap.open(cap_id);
    }
    if (!cap.isOpened()) {
        cout << "ERROR: could not open video capture" << endl;
        return -1;
    }

    GET_CAP_PROP(CV_CAP_PROP_POS_MSEC);
    GET_CAP_PROP(CV_CAP_PROP_POS_FRAMES);
    GET_CAP_PROP(CV_CAP_PROP_POS_AVI_RATIO);
    GET_CAP_PROP(CV_CAP_PROP_FRAME_WIDTH);
    GET_CAP_PROP(CV_CAP_PROP_FRAME_HEIGHT);
    GET_CAP_PROP(CV_CAP_PROP_FPS);
    GET_CAP_PROP(CV_CAP_PROP_FOURCC);
    GET_CAP_PROP(CV_CAP_PROP_FRAME_COUNT);
    GET_CAP_PROP(CV_CAP_PROP_FORMAT);
    GET_CAP_PROP(CV_CAP_PROP_MODE);
    GET_CAP_PROP(CV_CAP_PROP_BRIGHTNESS);
    GET_CAP_PROP(CV_CAP_PROP_CONTRAST);
    GET_CAP_PROP(CV_CAP_PROP_SATURATION);
    GET_CAP_PROP(CV_CAP_PROP_HUE);
    GET_CAP_PROP(CV_CAP_PROP_GAIN);
    GET_CAP_PROP(CV_CAP_PROP_EXPOSURE);
    GET_CAP_PROP(CV_CAP_PROP_CONVERT_RGB);
    //GET_CAP_PROP(CV_CAP_PROP_WHITE_BALANCE);
    GET_CAP_PROP(CV_CAP_PROP_RECTIFICATION);


    amv_config config;
    amv_config_init(&config);
    load_amv_config(argv[1], &config);

    amv_debug_info debug;
    amv_debug_init(&debug);

config.black_cutoff = 40;
//config.angle_method = 0;
    config.image_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    config.image_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int n = 0;
    amv_state state;
    amv_state_new(&state, &config);
    namedWindow("res",1);
    for (;;)
    {
        Mat frame;
        cap >> frame;
        Mat debug_out = frame.clone();
        debug.debug_results = debug_out.ptr();
        amv_find_teams(frame.ptr(), &state, &debug);
        imshow("res", debug_out);
        if (waitKey(30) >= 0) break;
        n++;
    }
    cout << n << " frames shown" << endl;
    return 0;
}
