#ifndef HEADER_H
#define HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

#define AMV_MAX_ROBOTS 256

struct amv_image_pos {
    int x;
    int y;
};

struct amv_config {
    unsigned char* image;
    int height, width;
    
    struct amv_image_pos* white_points;
    int white_points_len;
    
    struct amv_image_pos* mask_points;
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

struct amv_robot_data {
    struct amv_image_pos position;
    double angle;
};

struct amv_team_data {
    int team_len;
    struct amv_robot_data team[AMV_MAX_ROBOTS];
};

struct amv_vision_data {
    struct amv_team_data blue_team;
    struct amv_team_data yellow_team;
    struct amv_image_pos ball_pos;
};

struct amv_vision_data amv_find_teams(struct amv_config* config);
void amv_init_config(struct amv_config* config);
void amv_free_config(struct amv_config* config);

#ifdef __cplusplus
}
#endif

#endif
