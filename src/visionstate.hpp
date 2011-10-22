#ifndef VISIONSTATE_H
#define VISIONSTATE_H
#include "hsvconverter.hpp"
#include "mask.hpp"

class VisionState {
public:
    HSVConverter converter;
    ImageMask mask;
};

struct amv_state {
    amv_config config;
    VisionState state;
};


#endif
