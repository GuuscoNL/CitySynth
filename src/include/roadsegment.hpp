#pragma once
#include <raylib.h>
#include <vector>

class RoadSegment {
public:
    RoadSegment(int delay, Shader shader, Vector2 fromPos, Vector2 toPos);
    ~RoadSegment();
    void Draw();
    int GetDelay() const;
    friend bool operator>(const RoadSegment lhs, const RoadSegment rhs);
    friend bool operator<(const RoadSegment lhs, const RoadSegment rhs);

private:
    Model model;
    int delay;
    Shader shader;

    Vector3 pos;
    Vector2 fromPos;
    Vector2 toPos;
    float angle;
    float length;
    RoadSegment* previousRoad;
    std::vector<RoadSegment*> nextRoads;
    void CalculatePosAndAngle();


};