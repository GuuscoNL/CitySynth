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

    UpdatePlaneTexture();
}

City::~City() {
    PRINT("Unloading City");
    UnloadTexture(populationHeatmap);
    UnloadModel(plane);

    for (auto* road : roads) {
        delete road;
    }
    roads.clear();

}

void City::Draw() {

    for (auto* road : roads) {
        road->Draw();
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
    DrawCylinder(Vector3{0, 0, 0}, 0.5, 0.5, 0.3, 10, GRAY);
    DrawCylinder(Vector3{20, 0, 15}, 0.5, 0.5, 0.3, 10, WHITE);
    // DrawGrid(size/10, 10.0f);
}

Texture2D City::GeneratePopulationHeatmap(int offsetX, int offsetY, float scale) {
    Image populationHeatmapImage = GenImagePerlinNoise(size, size, offsetX, offsetY, scale);
    populationHeatmap = LoadTextureFromImage(populationHeatmapImage);
    UnloadImage(populationHeatmapImage);
    UpdatePlaneTexture();
    return populationHeatmap;
}

void  City::GenerateCity(int amount) {
    std::priority_queue<RoadSegment, std::vector<RoadSegment>, RoadSegmentGreaterThan> Q;


    roads.push_back(new RoadSegment(0, shader, Vector2{0,0}, Vector2{15,20}));

    // for (int i = 0; i < amount; i++) {
    //     roads.push_back(new RoadSegment(roadModel, shader, Vector3{ (float)(0 + i), 0, 0 }));
    // }
}

void City::UpdatePlaneTexture() {
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = populationHeatmap;
}

Texture2D City::GetPopulationHeatmap() const {
    return populationHeatmap;
}

Model City::GetPlane() const {
    return plane;
}