#ifndef AREA_H
#define AREA_H

#include "defs.hpp"
#include "amv.h"
#include "array2d.hpp"
#include "mshift.hpp"
#include "moments.hpp"

#include <vector>

class Patch;
class PatchType;


class PatchFinder {
    template <typename F>
    class PrecomputeFun {
    public:
        Image img;
        F f;
        PatchType* operator()(int x, int y, PatchType*) {
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
    amv_state* state;
    PatchFinder(amv_state* state){
        this->state = state;
    }
    void setImages(Image img, Image img_hsv);
    
    bool isIn(int x, int y);
    void meanShift();
    void getSets();

    static int colorDistance(cv::Vec3b a, cv::Vec3b b);
};

class PatchType {
public:
    typedef bool(*Fun)(amv_config*, cv::Vec3b);
    Fun fun;
    cv::Vec3b color;
    amv_config* config;
	PatchType(PatchFinder* pf, Fun fun, cv::Vec3b color, amv_config* config) {
		this->map = pf;
	    this->fun = fun;
	    this->color = color;
	    this->config = config;
	}
	~PatchType();

	PatchFinder* map;
	std::vector<Patch*> patches;
	Patch* newPatch();

	void fillTeam(amv_team_data* data);

	int getMinPatchSize();
    int getMaxPatchSize();
    int getMaxPatchWidth();
};

class Patch {
    cv::Vec3b origin;
    cv::Rect aabbox;
public:
    PatchMoments moments;
	PatchType* type;
	Patch(PatchType*t) {
	    assert(t);
	    type = t;
	}
	bool add(cv::Point p, cv::Point neighbour);
	int getCount();
	bool isLegal();
	cv::Point getMean();
	double getAngle();
	cv::Rect getBoundingBox();
};
#endif
