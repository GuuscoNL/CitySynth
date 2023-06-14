#pragma once
#include <raylib.h>
#include "intersection.hpp"

class RoadSegment {
public:
    RoadSegment(Shader shader, Intersection* from, Intersection* to);
    ~RoadSegment();
    void Draw();

private:
    Model model;
    Shader shader;
    Intersection* from;
    Intersection* to;
    Vector3 pos;
    float angle;
    float length;
    RoadSegment* previous;
    RoadSegment* next;
    void CalculatePosAndAngle();


};