#pragma once
#include <raylib.h>
#include "simplexnoise.hpp"

struct Settings {

    // Highway
    float highwayLength;
    float highwayWidth;
    float highwayHeight;
    float highwayAngle;
    int highwaySampleAmount;
    float highwayBranchChange;

    // Simplex
    float frequency;// bigger = smaller patches
    float amplitude;// ?
    float lacunarity;// ?
    float persistence;// ?
    float octaves;// More = more blurry

    // Misc
    Shader shader;

private:
    int randomSeed;

public:
    void SetSeed(int seed) {
        randomSeed = seed;
        SetRandomSeed(seed);
    }
};