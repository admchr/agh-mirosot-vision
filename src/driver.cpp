
#include "amv.h"
#include "common.hpp"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;


void teamOutput(amv_team_data* team) {
    cout<<team->team_len<<endl;
    for (int i=0; i<team->team_len; i++) {
        amv_robot_data robot = team->team[i];
        amv_image_pos p = robot.position;
        cout<<p.x<<" "<<p.y<<" "<<robot.angle<<" ["<<robot.color[0]+0<<", "<<robot.color[1]+0<<"] "<<robot.certainty<<endl;
    }
}

void process(string in_fname, string out_fname, amv_state* state)
{
    cv::Mat_<cv::Vec3b> img0 = cv::imread(in_fname);
    cv::Mat_<cv::Vec3b> img_copy = img0.clone();
    cv::Mat_<cv::Vec3b> img_white(img0.clone()), img_prescreen(img0.clone()), img_mshift(img0.clone()), img_patches(img0.clone()), img_robots(img0.clone()), img_results(img0.clone());

    amv_debug_info debug;
    amv_debug_init(&debug);
    debug.debug_balance = img_white.ptr();
    debug.debug_meanshift = img_mshift.ptr();
    debug.debug_prescreen = img_prescreen.ptr();
    debug.debug_patches = img_patches.ptr();
    debug.debug_robots = img_robots.ptr();
    debug.debug_results = img_results.ptr();

    amv_vision_data data = amv_find_teams(img0.ptr(), state, &debug);

    cout<<data.ball_pos.x<<" "<<data.ball_pos.y<<endl;
    teamOutput(&data.blue_team);
    teamOutput(&data.yellow_team);

    cv::imwrite(out_fname + "_0orig.png", img_copy);
    cv::imwrite(out_fname + "_1white.png", img_white);
    cv::imwrite(out_fname + "_2prescreen.png", img_prescreen);
    cv::imwrite(out_fname + "_3mshift.png", img_mshift);
    cv::imwrite(out_fname + "_4patches.png", img_patches);
    cv::imwrite(out_fname + "_5robots.png", img_robots);
    cv::imwrite(out_fname + "_6results.png", img_results);
}

int main(int argc, char**argv) {
    amv_config config;
    amv_config_init(&config);
    string in_fname;
    string config_fname;
    string out_fname;
    
    amv_state state;


    if (argc!=1) {
        in_fname = argv[1];
        config_fname = argv[2];
        out_fname = argv[3];
        load_config(&config, config_fname.c_str());
        cv::Mat_<cv::Vec3b> img0 = cv::imread(in_fname);
        config.image_width = img0.size().width;
        config.image_height = img0.size().height;

        amv_state_new(state, config);
        process(in_fname, out_fname, &state);
    } else {
        int n;
        cin>>n;
        cin>>config_fname;
        load_config(&config, config_fname.c_str());
        bool initialized = false;
        for (int i=0;i<n;i++) {
            cin>>in_fname>>out_fname;
            if (!initialized) {
                initialized = true;
                cv::Mat_<cv::Vec3b> img0 = cv::imread(in_fname);
                config.image_width = img0.size().width;
                config.image_height = img0.size().height;
                amv_state_new(state, config);
            }
            process(in_fname, out_fname, &state);
        }
    }
    amv_state_free(&state);

    return 0;
}
