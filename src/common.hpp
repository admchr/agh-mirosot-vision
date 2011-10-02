
#include <fstream>
void load_config(mirosot_vision_config* config, const char* fname) {
    std::ifstream configfile(fname);
    
    int white_len;
    configfile>>white_len;
    config->white_points_len = white_len;
    
    image_pos* tab_w = new image_pos[white_len];
    for (int i=0;i<white_len;i++){
        configfile>>tab_w[i].x>>tab_w[i].y;
    }
    config->white_points = tab_w;

    int mask_len;
    configfile>>mask_len;
    config->mask_points_len = mask_len;

    image_pos* tab_m = new image_pos[mask_len];
    for (int i=0;i<mask_len;i++){
        configfile>>tab_m[i].x>>tab_m[i].y;
    }
    config->mask_points = tab_m;
}
