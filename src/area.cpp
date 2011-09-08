
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


class AreaFinder {
public:
    PatchFinder *a;
    PatchType* operator()(int orig_x, int orig_y, PatchType* b) {
        if (!b) return false;

        if (a->area_map.get(orig_x, orig_y) != 0)
            return 0;
        Patch* pt = b->newPatch();

        std::queue<Point> Q;
        Q.push(Point(orig_x, orig_y));
        while (!Q.empty()) {
            Point p = Q.front();
            Q.pop();



            Vec3b color = a->img(p);
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = p.x+dx;
                    int ny = p.y+dy;
                    if (nx<0 || ny<0 || nx>=a->img.cols || ny>= a->img.rows)
                        continue;
                    a->preparePixel(Point(nx, ny));
                    if (a->area_map.get(nx, ny))
                    	continue;
                    if (!pt->add(Point(nx, ny), p)) continue;
                    a->area_map.set(nx, ny, pt);

                    Q.push(Point(nx, ny));
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

bool is_lil_blue(Vec3b c){
    return c[2]>120 && c[0]>85 && c[0]<115 && c[1]*c[2]>128*128/4;
}
bool Patch::add(cv::Point p, cv::Point neighbour){

	Image img = this->type->map->img;
	Image hsv = this->type->map->img_hsv;
    if (count < type->getMaxPatchSize()) {
    	if (count == 0) origin = img(neighbour);
    	if (2*PatchFinder::colorDistance(img(p), origin) > 240*100 || PatchFinder::colorDistance(img(p), img(neighbour)) > 4*100)
    		return false;
    	if (!is_lil_blue(hsv(p)))
    		return false;
    	count++;
    	return true;
    }
    return false;
}
int Patch::getCount() {
    return count;
}

