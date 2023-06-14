#include <iostream>
#include "include/intersection.hpp"

#define PRINT(x) std::cout << x << std::endl

Intersection::Intersection(Vector3 pos, Shader shader) :
    pos(pos),
    shader(shader) {
    model = LoadModelFromMesh(GenMeshCylinder(0.3, 0.11, 10));
    model.materials[0].shader = shader;

}

Intersection::~Intersection() {
    PRINT("Unloading Intersection");
    UnloadModel(model);
}

void Intersection::Draw() {
    DrawModel(model, pos, 1, GRAY);
}

Vector3 Intersection::GetPos() {
    return pos;
}