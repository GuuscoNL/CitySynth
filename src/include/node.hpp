#pragma once
#include "raylib.h"
#include <vector>
#include "settings.hpp"

class RoadSegment;

class Node {
public:
    Node(const Vector2& pos2D, Settings* settings);
    void Draw() const;
    Vector2 GetPos() const;
    int GetSize() const;
    void AddRoad(RoadSegment* road);
    void RemoveRoad(const RoadSegment* road);
    std::vector<RoadSegment*> GetConnectedRoads() const;
    Color color;
    int id;
    static int nextId;

private:
    std::vector<RoadSegment*> connectedRoads;
    Vector2 pos2D;
    Vector3 pos3D;
    Settings* settings;
    Model model;
};