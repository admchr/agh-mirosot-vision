
#include "area.hpp"

#include "mshift.hpp"
#include <opencv/cv.h>


using namespace cv;

void Area::setImage(Image img) {
    this->img = img;
    cv::Size size = img.size();
    set.resize(size);
    size.width = size.width/TILE_SIZE;
    size.height = size.height/TILE_SIZE;
    tile_set.resize(size);
    
}
#include <iostream>
bool Area::isIn(int x, int y) {
    return set.get(x, y);
}

class ShiftMeanInTile {
public:
	Image img;
    Area* a;
    bool operator()(int x, int y, bool on) {
        int ts = a->TILE_SIZE;
        int ov = a->OVERLAP;
        if (on) {
        	Rect roi = Rect(x*ts-ov, y*ts-ov, ts+2*ov, ts+2*ov) & Rect(0, 0, img.cols, img.rows);
            Image tmp = img(roi);
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
void Area::meanShift(Image img){
    ShiftMeanInTile sh;
    sh.img = img;
    sh.a = this;
    tile_set.forEach(sh);
}
