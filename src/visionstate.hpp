#ifndef AMV_VISIONSTATE_HPP
#define AMV_VISIONSTATE_HPP
#include "hsvconverter.hpp"
#include "mask.hpp"
#include "patch.hpp"

class VisionState {
public:
    ImageMask mask;
    PatchFinder area;
};

#endif
