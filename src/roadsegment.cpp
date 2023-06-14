#include <iostream>
#include "include/roadsegment.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl
#define PRINTVEC3(vec) std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl

RoadSegment::RoadSegment(Shader shader, Intersection* from, Intersection* to) :
    shader(shader),
    from(from),
    to(to) {

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

void RoadSegment::CalculatePosAndAngle() {
    Vector2 fromVec2D = Vector2{ from->GetPos().z, from->GetPos().x };
    Vector2 toVec2D = Vector2{ to->GetPos().z, to->GetPos().x };

    float x = (fromVec2D.y + toVec2D.y) / 2;
    float z = (fromVec2D.x + toVec2D.x) / 2;

    pos = Vector3{ x, 0.05, z };
    angle = (RAD2DEG * Vector2Angle(fromVec2D, toVec2D));
    length = Vector2Distance(fromVec2D, toVec2D);
}