#ifndef HEADER_H
#define HEADER_H

#define MAX_ROBOTS 256

struct mirosot_vision_conf {
    unsigned char *image;
    int height, width;
    
    int meanshift_radius;
    int meanshift_threshold;
    
    unsigned char *debug_balance;
    unsigned char *debug_meanshift;
};

struct image_pos {
    int x;
    int y;
};

struct robot_data {
    int team1_len;
    image_pos team1[MAX_ROBOTS];
    
    int team2_len;
    image_pos team2[MAX_ROBOTS];
    
    image_pos ball_pos;
};

robot_data find_teams(mirosot_vision_conf*cnf);
void init_config(mirosot_vision_conf*cnf);
#endif
