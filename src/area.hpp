#ifndef AREA_H
#define AREA_H

#include "header.h"
#include "set2d.hpp"

#include <vector>
#include <opencv/cv.h>

typedef cv::Mat_<cv::Vec3b> Image;



class Area {
    
template <typename F>
class PrecomputeFun {
public:
    Area *a;
    F f;
    bool operator()(int x, int y, bool b) {
        bool ret = f(a->img(y, x));
        if (ret) {
            int ts = Area::TILE_SIZE;
            int xt = x/ts;
            int yt = y/ts;
            for (int dx = -1; dx <= 1; dx++) 
                for (int dy = -1; dy <= 1; dy++)
                    a->tile_set.set(xt + dx, yt + dy, true);
        
        }
        return ret;
    }
};
public:
    Set2d<bool> set;
    Set2d<bool> tile_set;
    Image img;
    static const int TILE_SIZE = 16;
    void setImage(Image img);
    
    template <typename F>
    void precompute(F f) {
        PrecomputeFun<F> pf;
        pf.f = f;
        pf.a = this;
        set.forEach(pf);
    }
    
    bool isIn(int x, int y);
};



#endif
