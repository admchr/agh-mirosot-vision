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
#define AMV_MAX_SECONDARY_COLORS 3
#define AMV_MAX_ANGLE_METHOD 5

struct AMV_EXPORT amv_image_pos {
    int x;
    int y;
};

struct AMV_EXPORT amv_point {
    double x;
    double y;
};

struct AMV_EXPORT amv_color_info {
    int hue_min;
    int hue_max;
};

struct AMV_EXPORT amv_robot_info {
    int front_color;
    int back_color;
};

struct AMV_EXPORT amv_team_info {
    struct amv_color_info color;
    int team_size;

    int home_team;
    struct amv_robot_info robot_info[AMV_MAX_ROBOTS];
    struct amv_color_info secondary_colors[AMV_MAX_SECONDARY_COLORS];
};

struct amv_transform_info {
    struct amv_image_pos field_top_left;
    struct amv_image_pos field_bottom_right;
    struct amv_point output_scale;
};

struct AMV_EXPORT amv_config {
    int image_height, image_width;

    struct amv_image_pos* white_points;
    int white_points_len;
    
    struct amv_image_pos* mask_points;
    int mask_points_len;

    double px_per_cm;
    
    int meanshift_radius;
    int meanshift_threshold;
    int same_color_distance;
    int linearize;

    struct amv_transform_info transform;

    int black_cutoff;
    struct amv_team_info blue;
    struct amv_team_info yellow;
    struct amv_color_info orange;
    int white_is_yellow;
    int minimum_saturation;
    int white_cutoff;

    int angle_method;
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
    unsigned char *debug_results;
};

struct AMV_EXPORT amv_robot_data {
    struct amv_point position;
    int identity;
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
    struct amv_point ball_pos;
};

struct AMV_EXPORT amv_state {
    struct amv_config* config;
    void* state;
};
struct amv_vision_data AMV_EXPORT
    amv_find_teams(
        unsigned char* image,
        struct amv_state* state,
        struct amv_debug_info* debug
    );

void AMV_EXPORT amv_vertical_flip(unsigned char* image, int width, int height);

void AMV_EXPORT amv_config_init(struct amv_config* config);
void AMV_EXPORT amv_debug_init(struct amv_debug_info* debug);

void AMV_EXPORT amv_state_new(struct amv_state* st, struct amv_config* config);
void AMV_EXPORT amv_state_free(struct amv_state* state);


void AMV_EXPORT load_amv_config(const char* xmlFile, struct amv_config* config);
int AMV_EXPORT save_amv_config(const char* xmlFile, struct amv_config* config);

#ifdef __cplusplus
}
#endif

#endif
