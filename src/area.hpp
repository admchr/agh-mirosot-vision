#ifndef AREA_H
#define AREA_H

#include "defs.hpp"
#include "header.h"
#include "array2d.hpp"
#include "pixelset.hpp"
#include "mshift.hpp"

#include <vector>

class Patch;
class PatchType;


class PatchFinder {
    template <typename F>
    class PrecomputeFun {
    public:
        Image img;
        F f;
        PatchType* operator()(int x, int y, PatchType* b) {
            return f(img(y, x));
        }
    };
public://TODO make some private
    template <typename F>
    void precompute(F f) {
        PrecomputeFun<F> pf;
        pf.f = f;
        pf.img = img_hsv;
        precompute_map.forEach(pf);
    }
	void preparePixel(cv::Point p);

    Array2d<bool> meanshifted;

    Array2d<PatchType*> precompute_map;
    Array2d<Patch*> area_map;
    Image img;
    Image img_hsv;
    mirosot_vision_config config;
    PatchFinder(mirosot_vision_config config){
        this->config = config;
    }
    void setImages(Image img, Image img_hsv);
    
    bool isIn(int x, int y);
    void meanShift();
    void getSets();

    static int colorDistance(cv::Vec3b a, cv::Vec3b b);
};

class PatchType {
public:
    typedef bool(*Fun)(cv::Vec3b);
    Fun fun;
	PatchType(PatchFinder* pf, Fun fun) {
		this->map = pf;
	    this->fun = fun;
	}
	~PatchType();

	PatchFinder* map;
	std::vector<Patch*> patches;
	Patch* newPatch();

	int getMinPatchSize();
	int getMaxPatchSize();
};

class Patch {
	PatchType* type;
	int count;
	cv::Vec3b origin;
public:
	Patch(PatchType*t) {
	    count = 0;
	    type = t;
	}
	bool add(cv::Point p, cv::Point neighbour);
	int getCount();
	bool isLegal();
};
#endif
