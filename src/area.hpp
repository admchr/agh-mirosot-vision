#ifndef AREA_H
#define AREA_H

#include "defs.hpp"
#include "amv.h"
#include "array2d.hpp"
#include "mshift.hpp"
#include "moments.hpp"

#include <vector>

class Patch;

void fillTeam(std::vector<Patch*> team, amv_team_data* data);
void fillBall(Patch* ball, amv_vision_data* data);

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
    amv_color_info team;
    cv::Vec3b color;
    amv_config* config;
	PatchType(PatchFinder* pf, amv_color_info team, cv::Vec3b color, amv_config* config) {
		this->map = pf;
	    this->team = team;
	    this->color = color;
	    this->config = config;
	}
	~PatchType();

	PatchFinder* map;
	std::vector<Patch*> patches;
	Patch* newPatch();

    std::vector<Patch*> getTeam();
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
    void getSecondaryPatches(int* out, Image* debug=0);
    cv::Point getCenter();
	double getAngle();
	cv::Rect getBoundingBox();
};
#endif
