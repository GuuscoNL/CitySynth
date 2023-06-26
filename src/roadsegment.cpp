#include <iostream>
#include "include/roadsegment.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl

RoadSegment::RoadSegment(int delay, Settings* settings, Node* from, Node* to) :
    delay(delay),
    settings(settings),
    from(from),
    to(to) {
}

RoadSegment::~RoadSegment() {
    from->RemoveRoad(this);
    to->RemoveRoad(this);
}

void RoadSegment::Draw() const {
    DrawModelEx(model, pos, { 0, 1, 0 }, angle, Vector3{ 1, 1, length / modelLength }, color);
}

std::ostream& operator<<(std::ostream& os, const RoadSegment& n) {
    os << "FROM: " << n.GetFromPos().x << ", " << n.GetFromPos().y;
    os << "  TO: " << n.GetToPos().x << ", " << n.GetToPos().y << std::endl;
    return os;
}

int RoadSegment::GetDelay() const {
    return delay;
}

Vector2 RoadSegment::GetToPos() const {
    return to->GetPos();
}

Vector2 RoadSegment::GetFromPos() const {
    return from->GetPos();
}

Node* RoadSegment::GetFrom() const {
    return from;
}
Node* RoadSegment::GetTo() const {
    return to;
}

void RoadSegment::SetDelay(int delay) {
    this->delay = delay;
}

float RoadSegment::GetAngle() const {
    return angle;
}

RoadSegment::RoadType RoadSegment::GetType() const {
    return type;
}

void RoadSegment::SetFrom(Node* node) {
    from->RemoveRoad(this);
    from = node;
    node->AddRoad(this);
    CalculatePosAndAngle();
}

void RoadSegment::SetTo(Node* node) {
    to->RemoveRoad(this);
    to = node;
    node->AddRoad(this);
    CalculatePosAndAngle();
}

void RoadSegment::SetColor(Color color) {
    this->color = color;
}

void RoadSegment::CalculatePosAndAngle() {

    float y = (GetFromPos().y + GetToPos().y) / 2;
    float x = (GetFromPos().x + GetToPos().x) / 2;

    // VEC2 {x, y}
    // VEC3 {y, z, x}
    pos = Vector3{ y, height / 2, x };


    angle = (RAD2DEG * Vector2Angle(GetFromPos(), GetToPos()));

    length = Vector2Distance(GetFromPos(), GetToPos());
}