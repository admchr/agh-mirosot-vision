#ifndef AREA_H
#define AREA_H

#include "header.h"

#include <vector>
#include <opencv/cv.h>

typedef cv::Mat_<cv::Vec3b> Image;


class Area {
    std::vector<bool> set;
    std::vector<bool> tile_set;
    Image img;
public:
    static const int TILE_SIZE = 16;
    void setImage(Image img);
    
    template <typename F>
    void precompute(F f);
    
    bool isIn(int x, int y);
};

template <typename F>
void Area::precompute(F f) {
    for (int x=0;x<img.size().width;x++)
        for (int y=0;y<img.size().height;y++){
            
            if (f(img(y, x))) {
                int tile_x=x/TILE_SIZE, tile_y=y/TILE_SIZE;
                set[img.size().width*y + x]=true;
                
            }
        }
    
}


#endif
