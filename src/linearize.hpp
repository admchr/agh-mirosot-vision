#ifndef AMV_LINEARIZE_HPP
#define AMV_LINEARIZE_HPP

#include "defs.hpp"

uchar srgb_to_rgb(uchar ch);
uchar rgb_to_srgb(uchar ch);

void from_sRGB_to_RGB(Image& img);
void from_RGB_to_sRGB(Image& img);




#endif
