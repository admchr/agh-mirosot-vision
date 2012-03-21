#ifndef AMV_VISIONSTATE_HPP
#define AMV_VISIONSTATE_HPP
#include "hsvconverter.hpp"
#include "mask.hpp"
#include "area.hpp"

class VisionState {
public:
    ImageMask mask;
    PatchFinder area;
};

#endif
