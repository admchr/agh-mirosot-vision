
#include "area.hpp"

void Area::setImage(Image img) {
    int size = (img.size().height+1)*(img.size().width+1);
    this->img = img;
    set.resize(size);
    tile_set.resize(size/(TILE_SIZE*TILE_SIZE));
}

bool Area::isIn(int x, int y) {
    return set[img.size().width*y+x];
}
