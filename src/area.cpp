
#include "area.hpp"

void Area::setImage(Image img) {
    this->img = img;
    set.resize(img.size());
    tile_set.resize(img.size());//TODO
}

bool Area::isIn(int x, int y) {
    return set.get(x, y);
}
