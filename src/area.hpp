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
public://TODO make some private
	void preparePixel(cv::Point p) {
		if (meanshifted.get(p.x, p.y))
			return;
		meanShiftPoint(
					img,
					p.x,
					p.y,
					config.meanshift_radius, // position radius
					config.meanshift_threshold // value radius
				);
		meanshifted.set(p.x, p.y, true);
	}

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

	template <typename F>
	class PrecomputeFun {
	public:
	    Image img;
	    PatchType *a;
	    F f;
	    PatchType* operator()(int x, int y, PatchType* b) {
	        if (f(img(y, x))) return a;

	        return b;
	    }
	};
public:
	PatchType(PatchFinder* pf) {
		this->map = pf;
	}
	~PatchType();
    template <typename F>
    void precompute(F f, Image img) {
        PrecomputeFun<F> pf;
        pf.f = f;
        pf.a = this;
        pf.img = img;
        map->precompute_map.forEach(pf);
    }

	PatchFinder* map;
	std::vector<Patch*> patches;
	Patch* newPatch();

	int getMinPatchSize() {
	    double min_size = map->config.px_per_cm * 3.5 * 0.75;
	    int min_area = min_size*min_size;

	    return min_area;
	}
	int getMaxPatchSize() {
	    double max_size = map->config.px_per_cm * 7.5 * 1.25;
	    int max_area = max_size*max_size;

	    return max_area;
	}
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
	bool isLegal() {return count > type->getMinPatchSize() && count < type->getMaxPatchSize();}
};
#endif
