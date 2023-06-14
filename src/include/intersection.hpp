#pragma once
#include "raylib.h"
#include <vector>

class RoadSegment;

class Intersection {
public:
    Intersection(Vector3 pos, Shader shader);
    ~Intersection();
    void Draw();
    Vector3 GetPos();

private:
    std::vector<RoadSegment*> roads;
    Vector3 pos;
    Model model;
    Shader shader;

};