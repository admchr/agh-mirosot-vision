
#include "area.hpp"

void Area::setImage(Image img) {
    this->img = img;
    cv::Size size = img.size();
    set.resize(size);
    size.width = size.width/TILE_SIZE + 1;
    size.height = size.height/TILE_SIZE + 1;
    tile_set.resize(size);
    
}

bool Area::isIn(int x, int y) {
    return set.get(x, y);
}
