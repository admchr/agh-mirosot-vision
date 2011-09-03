#ifndef AREA_H
#define AREA_H

#include "header.h"
#include "array2d.hpp"
#include "pixelset.hpp"

#include <vector>

typedef cv::Mat_<cv::Vec3b> Image;


class Area {
public://TODO make some private

template <typename F>
class PrecomputeFun {
public:
    Image img;
    Area *a;
    F f;
    bool operator()(int x, int y, bool b) {
        bool ret = f(img(y, x));
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
    Array2d<bool> set;
    Array2d<bool> tile_set;

    Array2d<int> area_map;
    std::vector<PixelSet> area_vec;
    Image img;
    static const int TILE_SIZE = 16;
    static const int OVERLAP = 3;
    mirosot_vision_config config;
    Area(mirosot_vision_config config){
        this->config = config;
    }
    void setImage(Image img);
    
    template <typename F>
    void precompute(F f, Image img) {
        PrecomputeFun<F> pf;
        pf.f = f;
        pf.a = this;
        pf.img = img;
        set.forEach(pf);
    }
    
    bool isIn(int x, int y);
    void meanShift();
    std::vector<PixelSet> getSets();

    static int colorDistance(cv::Vec3b a, cv::Vec3b b);
};

#endif
