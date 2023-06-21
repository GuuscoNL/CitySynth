#include <iostream>
#include "include/roadsegment.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl
#define PRINTVEC3(vec) std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl

RoadSegment::RoadSegment(int delay, Settings* settings, Node* from, Node* to) :
    delay(delay),
    settings(settings),
    from(from),
    to(to) {}

RoadSegment::~RoadSegment() {
    // PRINT("Unloading Road");
    from->RemoveRoad(this);
    to->RemoveRoad(this);
}

void RoadSegment::Draw() {
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

Node* RoadSegment::GetFrom() {
    return from;
}
Node* RoadSegment::GetTo() {
    return to;
}

void RoadSegment::SetDelay(int delay) {
    this->delay = delay;
}

float RoadSegment::GetAngle() const {
    return angle;
}

RoadSegment::RoadType RoadSegment::GetType() {
    return type;
}

void RoadSegment::SetFrom(Node* node) {
    from = node;
    node->AddRoad(this);
    CalculatePosAndAngle();
}
void RoadSegment::SetTo(Node* node) {
    to = node;
    node->AddRoad(this);
    CalculatePosAndAngle();
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