#pragma once
#include <raylib.h>
#include <vector>
#include <iostream>
#include "settings.hpp"
#include "node.hpp"

class RoadSegment {
public:
    RoadSegment(int delay, Settings* settings, Node* from, Node* to);
    ~RoadSegment();
    void Draw();
    friend std::ostream& operator<<(std::ostream& os, const RoadSegment& n);
    int GetDelay() const;
    Vector2 GetToPos() const;
    Vector2 GetFromPos() const;
    float GetAngle() const;
    Node* GetFrom();
    Node* GetTo();
    void SetDelay(int delay);
    void SetFrom(Node* node);
    void SetTo(Node* node);

private:
    Model model;
    int delay;
    Settings* settings;
    Color color;

    Vector3 pos;
    Node* from;
    Node* to;
    float angle;
    float length;
    RoadSegment* previousRoad;
    std::vector<RoadSegment*> nextRoads;
    void CalculatePosAndAngle();
};