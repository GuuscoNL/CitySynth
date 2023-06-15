#include <iostream>
#include <queue>
#include "include/city.hpp"
#include "raymath.h"
#include "include/highway.hpp"
#include "include/roadsegmentgreaterthan.hpp"


#define PRINT(x) std::cout << x << std::endl

City::City(float size, Shader shader) :
    size(size),
    shader(shader) {
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));
    plane.materials[0].shader = shader;

    populationHeatmap = LoadTextureFromImage(GenImageColor(size, size, WHITE));
    heatmapCenter = Vector2{round(size/2), round(size/2)};

    UpdatePlaneTexture();
}

City::~City() {
    PRINT("Unloading City");
    UnloadTexture(populationHeatmap);
    UnloadModel(plane);

    ResetCity();
}

void City::Draw() {

    for (auto* road : roads) {
        road->Draw();
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
    DrawCylinder(Vector3{ 0, 0, 0 }, 0.5, 0.5, 0.3, 10, GRAY);
}

Texture2D City::GeneratePopulationHeatmap(int offsetX, int offsetY, float scale) {
    populationHeatmap = LoadTextureFromImage(GenImagePerlinNoise(size, size, offsetX, offsetY, scale));
    UpdatePlaneTexture();
    return populationHeatmap;
}

void  City::GenerateCity(unsigned int amount) {
    ResetCity();
    roads.reserve(amount); // speed!
    std::priority_queue<RoadSegment*, std::vector<RoadSegment*>, RoadSegmentGreaterThan> Q;
    Q.push(new RoadSegment(0, shader, Vector2{ 0,0 }, Vector2{ 2, 0 }));

    while (!Q.empty() && roads.size() < amount){
        RoadSegment* minRoad = Q.top();
        Q.pop();
        bool accepted = LocalConstraints(minRoad);
        if (accepted){
            roads.push_back(minRoad);
            PRINT(*minRoad);
            std::vector<RoadSegment*> newRoads = GlobalGoals(minRoad);
            for (auto* newRoad : newRoads){
                newRoad->SetDelay(minRoad->GetDelay() + 1 + newRoad->GetDelay());
                Q.push(newRoad);
            }
        } else{
            delete minRoad;
        }
    }

    // Make sure every non used roads gets deleted
    while (!Q.empty()){
        delete Q.top();
        Q.pop();
    }
    PRINT(roads.size());
}

bool City::LocalConstraints(RoadSegment* road){
    return true;
}

std::vector<RoadSegment*> City::GlobalGoals(RoadSegment* rootRoad){
    std::vector<RoadSegment*> newRoads;
    Vector2	newFromPos = rootRoad->GetToPos();
    Vector2 newToPos = GetPosWithAngle( newFromPos,
                                        rootRoad->GetAngle() + GetRandomValue(-20, 20));
    newRoads.push_back(new RoadSegment(1, shader, newFromPos, newToPos));
    return newRoads;
}

Vector2 City::GetPosWithAngle(Vector2 fromPos, float angle){
    float angleRad = angle * DEG2RAD;
    return Vector2{fromPos.x + cos(angleRad) * 3,
                                fromPos.y + sin(angleRad) * 3};
}

int City::GetPopulationFromHeatmap(Vector2 pos) const{
    Image heatmapImage = LoadImageFromTexture(populationHeatmap);
    GetImageColor(heatmapImage, heatmapCenter.x + pos.x, heatmapCenter.y + pos.y);
    UnloadImage(heatmapImage);
}

Vector2 City::ToTexVec2(Vector2 originalVec) const{
    return Vector2{heatmapCenter.x + round(originalVec.y), heatmapCenter.y + round(originalVec.x)};
}

void City::UpdatePlaneTexture() {
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = populationHeatmap;
}

void City::ResetCity() {
    for (auto* road : roads) {
        delete road;
    }
    roads.clear();
    roads.resize(0);
}

Texture2D City::GetPopulationHeatmap() const {
    return populationHeatmap;
}

Model City::GetPlane() const {
    return plane;
}