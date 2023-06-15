#pragma once
#include <raylib.h>
#include <vector>
#include <iostream>

class RoadSegment {
public:
    RoadSegment(int delay, Shader shader, Vector2 fromPos, Vector2 toPos);
    ~RoadSegment();
    void Draw();
    friend std::ostream& operator<<(std::ostream& os, const RoadSegment& n);
    int GetDelay() const;
    Vector2 GetToPos() const;
    Vector2 GetFromPos() const;
    float GetAngle() const;
    void SetDelay(int delay);

private:
    Model model;
    int delay;
    Shader shader;
    Color color;

    Vector3 pos;
    Vector2 fromPos;
    Vector2 toPos;
    float angle;
    float length;
    RoadSegment* previousRoad;
    std::vector<RoadSegment*> nextRoads;
    void CalculatePosAndAngle();


};