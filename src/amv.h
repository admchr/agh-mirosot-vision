#ifndef HEADER_H
#define HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

#define DLL __declspec(dllexport)

#define AMV_MAX_ROBOTS 256

struct DLL amv_image_pos {
    int x;
    int y;
};

struct DLL amv_config {
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
};

struct DLL amv_debug_info {
    unsigned char *debug_balance;
    unsigned char *debug_prescreen;
    unsigned char *debug_meanshift;
	char full_meanshift_debug;
    unsigned char *debug_patches;
    unsigned char *debug_robots;
};

struct DLL amv_robot_data {
    struct amv_image_pos position;
    double angle;
};

struct DLL amv_team_data {
    int team_len;
    struct amv_robot_data team[AMV_MAX_ROBOTS];
};

struct DLL amv_vision_data {
    struct amv_team_data blue_team;
    struct amv_team_data yellow_team;
    struct amv_image_pos ball_pos;
};

struct DLL amv_state {
    amv_config* config;
    void* state;
};
struct amv_vision_data  DLL
    amv_find_teams(
        unsigned char* image,
        struct amv_state* state,
        struct amv_debug_info* debug
    );


void DLL amv_config_init(struct amv_config* config);
void DLL amv_debug_init(struct amv_debug_info*);
void DLL amv_state_new(amv_state& st, struct amv_config& config);

void DLL amv_state_free(struct amv_state* state);

#ifdef __cplusplus
}
#endif

#endif
