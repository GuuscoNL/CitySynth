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
    float highwayCloseCrossing;
    float highwayCloseRoad;

    // Simplex
    float frequency;// bigger = smaller patches
    float amplitude;// ?
    float lacunarity;// ?
    float persistence;// ?
    float octaves;// More = more blurry

    // Model
    Model RoadModel;
    Model NodeModel;
    Shader shader;

private:
    int randomSeed;

public:


    void SetSeed(int seed) {
        randomSeed = seed;
        SetRandomSeed(seed);
    }
};