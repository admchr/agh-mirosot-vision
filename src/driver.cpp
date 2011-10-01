
#include "header.h"
#include "common.hpp"

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;
void teamOutput(team_data* team) {
    cout<<team->team_len<<endl;
    for (int i=0; i<team->team_len; i++) {
        robot_data robot = team->team[i];
        image_pos p = robot.position;
        cout<<p.x<<" "<<p.y<<" "<<robot.angle<<endl;
    }
}


void process(string in_fname, mirosot_vision_config & config, string config_name, string out_fname)
{
    cv::Mat_<cv::Vec3b> img0 = cv::imread(in_fname);
    cv::Mat_<cv::Vec3b> img_copy = img0.clone();
    config.width = img0.size().width;
    config.height = img0.size().height;
    config.image = img0.ptr();
    load_config(&config, config_name.c_str());
    cv::Mat_<cv::Vec3b> img_white(img0.clone()), img_prescreen(img0.clone()), img_mshift(img0.clone()), img_patches(img0.clone()), img_robots(img0.clone());
    config.debug_balance = img_white.ptr();
    config.debug_meanshift = img_mshift.ptr();
    config.debug_prescreen = img_prescreen.ptr();
    config.debug_patches = img_patches.ptr();
    config.debug_robots = img_robots.ptr();


    vision_data data = find_teams(&config);

    cout<<data.ball_pos.x<<" "<<data.ball_pos.y<<endl;
    teamOutput(&data.blue_team);
    teamOutput(&data.yellow_team);

    cv::imwrite(out_fname + "_0orig.png", img_copy);
    cv::imwrite(out_fname + "_1white.png", img_white);
    cv::imwrite(out_fname + "_2prescreen.png", img_prescreen);
    cv::imwrite(out_fname + "_3mshift.png", img_mshift);
    cv::imwrite(out_fname + "_4patches.png", img_patches);
    cv::imwrite(out_fname + "_5robots.png", img_robots);
}

int main(int argc, char**argv) {
    mirosot_vision_config config;
    init_config(&config);
    string in_fname;
    string config_fname;
    string out_fname;
    
    if (argc!=1) {
        in_fname = argv[1];
        config_fname = argv[2];
        out_fname = argv[3];
        process(in_fname, config, config_fname, out_fname);
    } else {
        int n;
        cin>>n;
        for (int i=0;i<n;i++) {
            cin>>in_fname>>config_fname>>out_fname;
            process(in_fname, config, config_fname, out_fname);
        }
    }

    return 0;
}
