
#include "area.hpp"

#include "mshift.hpp"
#include <opencv/cv.h>
#include <queue>

using namespace cv;

void PatchFinder::setImages(Image img, Image img_hsv) {
	this->img = img;
	this->img_hsv = img_hsv;
	cv::Size size = img.size();
    precompute_map.resize(size);
    meanshifted.resize(size);
    area_map.resize(size);
    
}

void PatchFinder::preparePixel(cv::Point p) {
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

class AreaFinder {
public:
    PatchFinder *a;
    std::queue<Point> Q;

    PatchType* operator()(int orig_x, int orig_y, PatchType* b) {
        if (!b) return false;

        if (a->area_map.get(orig_x, orig_y) != 0)
            return 0;
        Patch* pt = b->newPatch();
        Q.push(Point(orig_x, orig_y));
        while (!Q.empty()) {
            Point p = Q.front();
            Q.pop();

            Vec3b color = a->img(p);
            int minx = max(p.x - 1, 0);
            int miny = max(p.y - 1, 0);
            int maxx = min(p.x + 2, a->img.cols);
            int maxy = min(p.y + 2, a->img.rows);

            for (int nx = minx; nx < maxx; nx++)
                for (int ny = miny; ny < maxy; ny++) {
                    Point np = Point(nx, ny);
                    a->preparePixel(np);
                    if (a->area_map.get(nx, ny))
                    	continue;
                    if (!pt->add(np, p)) continue;
                    a->area_map.set(nx, ny, pt);

                    Q.push(np);
                }

        }


        return b;
    }
};

void PatchFinder::getSets() {
    AreaFinder f;
    f.a = this;
    precompute_map.forEach(f);
}

int PatchFinder::colorDistance(Vec3b a, Vec3b b) {
    int res=0;
    for (int i=0;i<3;i++) {
        int d = a[i] - b[i];
        res+=d*d;
    }
    return res;
}

PatchType::~PatchType() {
	for (int i=0; i<patches.size(); i++) {
		delete patches[i];
	}
}

Patch* PatchType::newPatch()
{
	patches.push_back(new Patch(this));
	return patches.back();
}

void PatchType::fillTeam(team_data* data) {
    data->team_len = 0;
    robot_data* robot = data->team;
    for (unsigned int i=0; i<patches.size(); i++) {
        Patch* patch = patches[i];
        if (patch->isLegal()) {
            cv::Point p = patch->getMean();
            robot->position.x = p.x;
            robot->position.y = p.y;
            robot->angle = patch->getAngle()+M_PI*0.25;
            robot++;
            data->team_len++;
        }
    }
}

int PatchType::getMinPatchSize() {
    double min_size = map->config.px_per_cm * 3.5 * 0.85;
    int min_area = min_size*min_size;

    return min_area;
}
int PatchType::getMaxPatchSize() {
    double max_size = map->config.px_per_cm * 7.5 * 1.25;
    int max_area = max_size*max_size;

    return max_area;
}

int PatchType::getMaxPatchWidth() {
    return map->config.px_per_cm * 8 * 2;
}

bool Patch::add(cv::Point p, cv::Point neighbour) {
	Image img = this->type->map->img;
	Image hsv = this->type->map->img_hsv;
    if (moments.getCount() < type->getMaxPatchSize()) {
    	if (moments.getCount() == 0) {
    	    origin = img(p);
    	    aabbox = Rect(p, p);
    	}
    	if (PatchFinder::colorDistance(img(p), img(neighbour)) > 4*100)
    		return false;
    	if (!type->fun(hsv(p)))
    		return false;
    	moments.add(p);
    	aabbox = aabbox | Rect(p, p);
    	return true;
    }
    return false;
}

int Patch::getCount() {
    return moments.getCount();
}
Rect Patch::getBoundingBox() {
    return aabbox;
}

bool Patch::isLegal() {
    return moments.getCount() > type->getMinPatchSize() && moments.getCount() < type->getMaxPatchSize();
}

Point Patch::getMean() {
    return moments.getMean();
}
double Patch::getAngle() {
    return moments.getAngle();
}