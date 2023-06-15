#include <iostream>
#include "include/roadsegment.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl
#define PRINTVEC3(vec) std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl

RoadSegment::RoadSegment(int delay, Shader shader, Vector2 fromPos, Vector2 toPos) :
    delay(delay),
    shader(shader),
    fromPos(fromPos),
    toPos(toPos) {
    PRINT("LOADING ROAD");

    CalculatePosAndAngle();
    model = LoadModelFromMesh(GenMeshCube(0.5, .1, length));
    this->model.materials[0].shader = shader;
    color = Color{ static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)) };
}

RoadSegment::~RoadSegment() {
    PRINT("Unloading Road");
    UnloadModel(model);
}

void RoadSegment::Draw() {
    DrawModelEx(model, pos, { 0, 1, 0 }, angle, Vector3One(), color);
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
    return toPos;
}

Vector2 RoadSegment::GetFromPos() const {
    return fromPos;
}

void RoadSegment::SetDelay(int delay) {
    this->delay = delay;
}

float RoadSegment::GetAngle() const {
    return angle;
}

void RoadSegment::CalculatePosAndAngle() {

    float y = (fromPos.y + toPos.y) / 2;
    float x = (fromPos.x + toPos.x) / 2;

    // VEC2 {x, y}
    // VEC3 {y, z, x}
    pos = Vector3{ y, 0.05, x };

    angle = (RAD2DEG * Vector2Angle(fromPos, toPos));
    PRINT(angle);
    length = Vector2Distance(fromPos, toPos);
}