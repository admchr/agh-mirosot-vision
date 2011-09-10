#ifndef HEADER_H
#define HEADER_H

#define MAX_ROBOTS 256

extern "C" {

struct image_pos {
    int x;
    int y;
};

struct mirosot_vision_config {
    unsigned char* image;
    int height, width;
    
    image_pos* white_points;
    int white_points_len;
    
    double px_per_cm;
    double robot_size;
    
    int meanshift_radius;
    int meanshift_threshold;
    
    unsigned char *debug_balance;
    unsigned char *debug_prescreen;
    unsigned char *debug_meanshift;
    unsigned char *debug_patches;
    void* state;
};

struct robot_data {
    int team1_len;
    image_pos team1[MAX_ROBOTS];
    
    int team2_len;
    image_pos team2[MAX_ROBOTS];
    
    image_pos ball_pos;
};

robot_data find_teams(mirosot_vision_config* config);
void init_config(mirosot_vision_config* config);
void free_config(mirosot_vision_config* config);
}
#endif
