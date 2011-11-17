#ifndef AMV_H
#define AMV_H

#ifdef __cplusplus
extern "C" {
#endif

#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64 || defined WINCE)
    #define AMV_EXPORT __declspec(dllexport)
#else
    #define AMV_EXPORT
#endif

#define AMV_MAX_ROBOTS 256

struct AMV_EXPORT amv_image_pos {
    int x;
    int y;
};

struct AMV_EXPORT amv_color_info {
    int hue_min;
    int hue_max;

    int captures_white;
};

struct AMV_EXPORT amv_config {
    int height, width;

    struct amv_image_pos* white_points;
    int white_points_len;
    
    struct amv_image_pos* mask_points;
    int mask_points_len;

    double px_per_cm;
    int team_size;
    
    int meanshift_radius;
    int meanshift_threshold;
    int same_color_distance;

    int black_cutoff;
    struct amv_color_info blue;
    struct amv_color_info yellow;
    struct amv_color_info orange;
    int team_hue[3];
    int minimum_saturation;
    int white_cutoff;
    char linearize;
};

struct AMV_EXPORT amv_debug_info {
    unsigned char *debug_balance;
    unsigned char *debug_prescreen;
    unsigned char *debug_meanshift;
    int full_meanshift_debug;
    int linear_meanshift;
    int multiple_meanshift;
    unsigned char *debug_patches;
    unsigned char *debug_robots;
};

struct AMV_EXPORT amv_robot_data {
    struct amv_image_pos position;
    int color[2];
    double angle;
    double certainty;
};

struct AMV_EXPORT amv_team_data {
    int team_len;
    struct amv_robot_data team[AMV_MAX_ROBOTS];
};

struct AMV_EXPORT amv_vision_data {
    struct amv_team_data blue_team;
    struct amv_team_data yellow_team;
    struct amv_image_pos ball_pos;
};

struct AMV_EXPORT amv_state {
    amv_config* config;
    void* state;
};
struct amv_vision_data AMV_EXPORT
    amv_find_teams(
        unsigned char* image,
        struct amv_state* state,
        struct amv_debug_info* debug
    );


void AMV_EXPORT amv_config_init(struct amv_config* config);
void AMV_EXPORT amv_debug_init(struct amv_debug_info*);
void AMV_EXPORT amv_state_new(amv_state& st, struct amv_config& config);

void AMV_EXPORT amv_state_free(struct amv_state* state);

#ifdef __cplusplus
}
#endif

#endif
