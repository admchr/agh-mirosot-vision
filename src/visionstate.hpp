#ifndef VISIONSTATE_H
#define VISIONSTATE_H
#include "hsvconverter.hpp"
#include "mask.hpp"
#include "area.hpp"

class VisionState {
public:
    ImageMask mask;
    PatchFinder area;
};

#endif
