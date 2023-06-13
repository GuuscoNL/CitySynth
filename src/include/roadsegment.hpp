#pragma once
#include <raylib.h>

class RoadSegment {
    public:
    RoadSegment(Model model, Vector3 pos=Vector3{0, 0, 0}, float angle=0);
    ~RoadSegment();
    void Draw();

    private:
    Model model;
    Vector3 pos;
    float angle;
    RoadSegment* previous;
    RoadSegment* next;
    
};