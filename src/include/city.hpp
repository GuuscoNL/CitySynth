#pragma once
#include <raylib.h>
#include <vector>
#include "roadsegment.hpp"
#include "settings.hpp"
#include "node.hpp"

class City {
public:
    City(float size, Settings* settings);
    ~City();
    void Draw();
    Texture2D GeneratePopulationHeatmap(int offsetX = 0, int offsetY = 0, float scale = 1.0f);
    void GenerateCity(unsigned int amount);
    Texture2D GetPopulationHeatmap() const;
    Model GetPlane() const;

private:
    std::vector<RoadSegment*> roads;
    std::vector<Node*> nodes;
    float size;
    Texture2D populationHeatmapTex;
    Image populationHeatmapImg;
    Vector2 heatmapCenter;
    Model plane;
    Settings* settings;

    void UpdatePlaneTexture();
    void ResetCity();
    bool LocalConstraints(RoadSegment* road);
    std::vector<RoadSegment*> GlobalGoals(RoadSegment* road);
    Vector2 GetPosWithAngle(Vector2 fromPos, float angle, float length);
    Vector2 HighwaySamples(Vector2 fromPos, float OriginalAngle, float MaxAngle);
    int GetPopulationFromHeatmap(Vector2 pos) const;
    bool RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection);
    float CrossProduct(Vector2 v1, Vector2 v2);
};