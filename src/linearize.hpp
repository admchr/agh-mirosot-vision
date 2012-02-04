#ifndef LINEARIZE_HPP_
#define LINEARIZE_HPP_

#include "defs.hpp"

uchar srgb_to_rgb(uchar ch);
uchar rgb_to_srgb(uchar ch);

void linearize(Image& img);
void delinearize(Image& img);




#endif
