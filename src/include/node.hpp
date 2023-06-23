#pragma once
#include "raylib.h"
#include <vector>
#include "settings.hpp"

class RoadSegment;

class Node {
public:
    Node(const Vector2& pos2D, Settings* settings);
    ~Node();
    void Draw();
    Vector2 GetPos() const;
    int GetSize();
    void AddRoad(RoadSegment* road);
    void RemoveRoad(RoadSegment* road);
    std::vector<RoadSegment*> GetConnectedRoads();
    Color color; // debug

private:
    std::vector<RoadSegment*> connectedRoads;
    Vector2 pos2D;
    Vector3 pos3D;
    Settings* settings;
    Model model;
};