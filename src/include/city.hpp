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
    Texture2D GeneratePopulationHeatmap(int offsetX = 0, int offsetY = 0);
    void GenerateCity(unsigned int amount);
    Texture2D GetPopulationHeatmap() const;
    Model GetPlane() const;
    std::vector<RoadSegment*> GetRoads();
    void ResetCity();
    void SetSize(int size);

private:
    std::vector<RoadSegment*> roads;
    std::vector<Node*> nodes;
    int size;
    Texture2D populationHeatmapTex;
    Image populationHeatmapImg;
    Vector2 heatmapCenter;
    Model plane;
    Settings* settings;

    void UpdatePlaneTexture();
    bool LocalConstraints(RoadSegment* road);
    void GlobalGoals(RoadSegment* road, std::vector<RoadSegment*>& newRoads);
    Vector2 GetPosWithAngle(const Vector2& fromPos, float angle, float length);
    Vector2 HighwaySamples(const Vector2& fromPos, float OriginalAngle, float MaxAngle);
    int GetPopulationFromHeatmap(const Vector2& pos) const;
    bool RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection);
    float CrossProduct(const Vector2& v1, const Vector2& v2);
    float DistNodeToRoad(Node* node, RoadSegment* road, Vector2& intersection);
    Node* AddIntersection(RoadSegment* toSplitRoad, RoadSegment* toAddRoad, const Vector2& intersectionPos);
};