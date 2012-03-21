#ifndef AMV_COMMON_HPP
#define AMV_COMMON_HPP

#include <fstream>
#include <iostream>
#include <cstdlib>

/** For testing purposes. */
void load_config(amv_config* config, const char* fname) {
    std::ifstream configfile(fname);
    
    int white_len;
    if (!(configfile>>white_len)) {
        std::cerr << "Could not read from config "<<fname<<"."<<std::endl;
        abort();
    }
    config->white_points_len = white_len;
    
    amv_image_pos* tab_w = new amv_image_pos[white_len];
    for (int i=0;i<white_len;i++){
        configfile>>tab_w[i].x>>tab_w[i].y;
    }
    config->white_points = tab_w;

    int mask_len;
    configfile>>mask_len;
    config->mask_points_len = mask_len;

    amv_image_pos* tab_m = new amv_image_pos[mask_len];
    for (int i=0;i<mask_len;i++){
        configfile>>tab_m[i].x>>tab_m[i].y;
    }
    config->mask_points = tab_m;
}

#endif
