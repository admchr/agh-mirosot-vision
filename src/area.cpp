
#include "area.hpp"

void Area::setImage(Image img) {
    this->img = img;
    set.resize(img.size().height*img.size().width);
    
}

bool Area::isIn(int x, int y) {
    return set[img.size().width*y+x];
}
