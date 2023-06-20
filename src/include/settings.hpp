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
    float highwayBranchChance;
    float highwaySideRoadBranchChance;
    float highwayCloseCrossing;
    float highwayCloseRoad;

    // SideRoad
    float sideRoadLength;
    float sideRoadWidth;
    float sideRoadHeight;
    float sideRoadBranchChance;
    float sideRoadBranchDelay;
    float sideRoadCloseCrossing;
    float sideRoadCloseRoad;
    float sideRoadThreshold;

    // Simplex
    float frequency;// bigger = smaller patches
    float amplitude;// ?
    float lacunarity;// ?
    float persistence;// ?
    float octaves;// More = more blurry

    // Model
    Model highwayModel;
    Model sideRoadModel;
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