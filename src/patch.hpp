#ifndef AMV_PATCH_HPP
#define AMV_PATCH_HPP

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
    void setImages(amv_state* state, Image img, Image img_hsv);
    
    bool isIn(int x, int y);
    void meanShift();
    void extendPatch(cv::Point np, cv::Point p, Patch* pt, std::vector<cv::Point>& Q);
    void getSets();

    static int colorDistance(cv::Vec3b a, cv::Vec3b b);
};

class PatchType {
public:
    amv_color_info team;
    cv::Vec3b color;
    amv_config* config;
    bool is_yellow;
	PatchType(PatchFinder* pf, amv_color_info team, cv::Vec3b color, amv_config* config, bool yellow) {
		this->map = pf;
	    this->team = team;
	    this->color = color;
	    this->config = config;
	    is_yellow = yellow;
	}
	~PatchType();

	PatchFinder* map;
	std::vector<Patch*> patches;
	Patch* newPatch();

    Patch* getBall();

    int getPatchSize();
	int getMinPatchSize();
    int getMaxPatchSize();
    int getMaxPatchWidth();
};

class Patch {
    cv::Vec3b origin;
    cv::Rect aabbox;
    cv::Vec3i color_sum;
public:
    PatchMoments moments;
	PatchType* type;
    bool isRobot;
    bool isBall;
	Patch(PatchType*t) {
	    assert(t);
	    type = t;
        isRobot = false;
        isBall = false;
	}
	bool add(cv::Point p, cv::Point neighbour);
	int getCount();
	cv::Vec3b getMeanColor();
    double getRobotCertainty();
    double getBallCertainty();
    cv::Point getRobotCenter();
    double getRobotAngle();
    cv::Point getCenter();
    double getAngle();
    double getAngleFitness(double angle, int DEBUG_ANGLE_METHOD, Image* debug = 0);
	cv::Rect getBoundingBox();
};
#endif
