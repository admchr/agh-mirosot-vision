
#include "area.hpp"

#include "mshift.hpp"
#include <opencv/cv.h>
#include <queue>

using namespace cv;

void Area::setImage(Image img) {
    this->img = img;
    cv::Size size = img.size();
    set.resize(size);
    area_map.resize(size);
    size.width = size.width/TILE_SIZE;
    size.height = size.height/TILE_SIZE;
    tile_set.resize(size);
    
}

bool Area::isIn(int x, int y) {
    return set.get(x, y);
}

class ShiftMeanInTile {
public:
    Area* a;
    bool operator()(int x, int y, bool on) {
    	Image tmp;
        int ts = a->TILE_SIZE;
        int ov = a->OVERLAP;
        if (on) {
        	Rect roi = Rect(x*ts-ov, y*ts-ov, ts+2*ov, ts+2*ov) & Rect(0, 0, a->img.cols, a->img.rows);
            tmp = a->img(roi);
            meanShiftFiltering(
                tmp,
                tmp,
                a->config.meanshift_radius, // position radius
                a->config.meanshift_threshold, // value radius
                0,
                cv::TermCriteria(CV_TERMCRIT_ITER, 5, 1.0)
            );
        }
        return on;
    }
};

void Area::meanShift(){
    ShiftMeanInTile sh;
    sh.a = this;
    tile_set.forEach(sh);
}

class AreaFinder {
public:
    Area *a;
    bool operator()(int orig_x, int orig_y, bool b) {
        if (!b) return false;

        if (a->area_map.get(orig_x, orig_y) != 0)
            return true;

        int area_index = a->area_vec.size();
        a->area_vec.push_back(PixelSet());

        std::queue<Point> Q;
        Q.push(Point(orig_x, orig_y));
        while (!Q.empty()) {
            Point p = Q.front();
            Q.pop();

            if (a->area_map.get(p.x, p.y) != 0)
                continue;

            a->set.set(p.x, p.y, true);
            a->area_vec[area_index].add(p, a->img(p));
            a->area_map.set(p.x, p.y, area_index);

            Vec3b color = a->img(p);
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = p.x+dx;
                    int ny = p.y+dy;
                    if (nx<0 || ny<0 || nx>=a->img.cols || ny>= a->img.rows)
                        continue;
                    if (Area::colorDistance(color, a->img(ny, nx))>10*10)
                        continue;
                    Q.push(Point(nx, ny));
                }

        }

        return true;
    }
};

std::vector<PixelSet> Area::getSets() {
    area_vec.clear();
    area_vec.push_back(PixelSet());

    AreaFinder f;
    f.a = this;
    set.forEach(f);
    return area_vec;
}

int Area::colorDistance(Vec3b a, Vec3b b) {
    int res=0;
    for (int i=0;i<3;i++) {
        int d = a[i] - b[i];
        res+=d*d;
    }
    return res;
}

