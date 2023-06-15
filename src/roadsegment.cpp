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

    CalculatePosAndAngle();
    model = LoadModelFromMesh(GenMeshCube(0.5, .1, length));
    this->model.materials[0].shader = shader;
}

RoadSegment::~RoadSegment() {
    PRINT("Unloading Road");
    UnloadModel(model);
}

void RoadSegment::Draw() {
    DrawModelEx(model, pos, { 0, 1, 0 }, angle, Vector3One(), WHITE);
}

int RoadSegment::GetDelay() const {
    return delay;
}

// VEC2 {x, y}
// VEC3 {y, z, x}

void RoadSegment::CalculatePosAndAngle() {

    float y = (fromPos.y + toPos.y) / 2;
    float x = (fromPos.x + toPos.x) / 2;

    pos = Vector3{ y, 0.05, x };

    angle = (RAD2DEG * Vector2Angle(fromPos, toPos));
    PRINT(angle);
    length = Vector2Distance(fromPos, toPos);
}

bool operator>(const RoadSegment lhs, const RoadSegment rhs){
    return lhs.GetDelay() > rhs.GetDelay();
}

bool operator<(const RoadSegment lhs, const RoadSegment rhs){
    return lhs.GetDelay() < rhs.GetDelay();
}