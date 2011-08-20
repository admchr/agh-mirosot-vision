#ifndef HEADER_H
#define HEADER_H

#define MAX_ROBOTS 256

struct mirosot_vision_conf {
    unsigned char *image;
    int height, width;
    
    int meanshift_something;
    
};

struct robot_pos {
    int x;
    int y;
};

struct robot_data {
    int team1_len;
    robot_pos team1[MAX_ROBOTS];
    
    int team2_len;
    robot_pos team2[MAX_ROBOTS];
    
    robot_pos ball_pos;
};

robot_data find_teams(mirosot_vision_conf*cnf);

#endif
