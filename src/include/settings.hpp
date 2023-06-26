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

    // Simple
    float frequency; //!< bigger = smaller patches
    float amplitude; //!< Effect unkown
    float lacunarity; //!< How "dense" the patches are?
    float persistence; //!< 1 / lacunarity
    float octaves; //!< 1 = smooth, otherwise rough?

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
    /**
     * @brief Construct a new Settings class, this will contain all the settings for
     * the City class.
     *
     * @param lightingShader The shader the RoadSegment and Node models will use.
     */
    Settings(Shader lightingShader);

    /**
     * @brief Set the Random Seed.
     * 
     * @param seed The seed
     */
    void SetSeed(int seed) {
        randomSeed = seed;
        SetRandomSeed(seed);
    }
};