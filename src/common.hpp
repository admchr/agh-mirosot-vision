
#include <fstream>
void load_config(mirosot_vision_config* config, const char* fname) {
    std::ifstream configfile(fname);
    
    int white_len;
    configfile>>white_len;
    config->white_points_len = white_len;
    
    image_pos* tab = new image_pos[white_len];
    for (int i=0;i<white_len;i++){
        configfile>>tab[i].x>>tab[i].y;
    }
    config->white_points = tab;
}
