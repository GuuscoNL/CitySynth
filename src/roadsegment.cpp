#include <iostream>
#include "include/roadsegment.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl

RoadSegment::RoadSegment(Model model, Vector3 pos, float angle):
model(model),
angle(angle){
    this->pos = Vector3Add(pos, Vector3{0, 0.05, 0});
}

RoadSegment::~RoadSegment(){
    PRINT("Unloading Road");
}

void RoadSegment::Draw(){
    DrawModelEx(model, pos, {0, 1, 0}, angle, Vector3One(), WHITE);
}
