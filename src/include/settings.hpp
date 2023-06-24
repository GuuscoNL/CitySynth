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

    // SideRoad
    float sideRoadLength;
    float sideRoadWidth;
    float sideRoadHeight;
    float sideRoadBranchChance;
    float sideRoadBranchDelay;
    float sideRoadCloseCrossing;
    float sideRoadCloseRoad;
    float sideRoadThreshold;

    // Road conflicts
    float CloseCrossing;
    float CloseRoad;

    // Simplex
    float frequency;// bigger = smaller patches
    float amplitude;// ?
    float lacunarity;// How "dense" the patches are?
    float persistence;// 1/lacunarity
    float octaves;// More = more blurry

    // Model
    Model highwayModel;
    Model sideRoadModel;
    Model NodeModel;
    Shader shader;

    // Misc
    bool ShowNodes;
    bool useCustomHeatmap;
    Image customHeatmap;

private:
    int randomSeed;

public:


    void SetSeed(int seed) {
        randomSeed = seed;
        SetRandomSeed(seed);
    }
};