
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
    Area* a;
    bool operator()(int x, int y, bool on) {
        int ts = a->TILE_SIZE;
        if (on) {
            Image tmp = (a->img)(Rect(x*ts, y*ts, ts, ts));
            CvMat mat = tmp;
            meanShiftFiltering(
                &mat,
                &mat,
                a->config.meanshift_radius, // position radius
                a->config.meanshift_threshold, // value radius
                0,
                cv::TermCriteria(CV_TERMCRIT_ITER, 5, 1.0)
            );
            tmp = Mat(&mat);
        }
        return on;
    }
};
void Area::meanShift(){
    ShiftMeanInTile sh;
    sh.a = this;
    tile_set.forEach(sh);
}