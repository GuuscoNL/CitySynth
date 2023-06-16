#pragma once
#include <raylib.h>
#include "simplexnoise.hpp"

struct Settings{
    Settings();
    float highwayLength;
    float highwayAngle;
    int AmountHighwaySamples;
    Shader shader;
};