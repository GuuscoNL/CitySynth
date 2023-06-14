#include <iostream>
#include "include/city.hpp"
#include "raymath.h"

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

    for (auto* intersection : intersections) {
        delete intersection;
    }
    intersections.clear();

}

void City::Draw() {

    for (auto* road : roads) {
        road->Draw();
    }

    for (auto* intersection : intersections) {
        intersection->Draw();
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
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

    intersections.push_back(new Intersection(Vector3Zero(), shader));
    intersections.push_back(new Intersection(Vector3{ 1, 0, 5 }, shader));
    roads.push_back(new RoadSegment(shader, intersections.at(0), intersections.at(1)));

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