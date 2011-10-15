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
    
    image_pos* mask_points;
    int mask_points_len;

    double px_per_cm;
    double robot_size;
    
    int meanshift_radius;
    int meanshift_threshold;

    int black_cutoff;
    int blue_min;
    int blue_max;
    int yellow_min;
    int yellow_max;
    int minimum_saturation;
    int white_cutoff;
    char linearize;

    unsigned char *debug_balance;
    unsigned char *debug_prescreen;
    unsigned char *debug_meanshift;
    unsigned char *debug_patches;
    unsigned char *debug_robots;
    void* state;
};

struct robot_data {
    image_pos position;
    double angle;
};

struct team_data {
    int team_len;
    robot_data team[MAX_ROBOTS];
};

struct vision_data {
    team_data blue_team;
    team_data yellow_team;
    image_pos ball_pos;
};

vision_data find_teams(mirosot_vision_config* config);
void init_config(mirosot_vision_config* config);
void free_config(mirosot_vision_config* config);
}
#endif
