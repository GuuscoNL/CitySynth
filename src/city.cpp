#include <iostream>
#include "include/city.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl

City::City(float size, Model roadModel):
    size(size),
    roadModel(roadModel){
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));

    Image heatmap = GenImageColor(size, size, WHITE);
    populationHeatmap = LoadTextureFromImage(heatmap);
    UnloadImage(heatmap);
    UpdatePlaneTexture();
    
}

City::~City(){
    PRINT("Unloading City");
    UnloadTexture(populationHeatmap);
    UnloadModel(plane);

    for (auto *road : roads){
        delete road;
    }
    roads.clear();

}

void City::Draw(){

    for (auto *road : roads){
        road->Draw();
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
    // DrawGrid(size/10, 10.0f);
}

Texture2D City::GeneratePopulationHeatmap(int offsetX, int offsetY, float scale){
    Image populationHeatmapImage = GenImagePerlinNoise(size, size, offsetX, offsetY, scale);
    populationHeatmap = LoadTextureFromImage(populationHeatmapImage);
    UnloadImage(populationHeatmapImage);
    UpdatePlaneTexture();
    return populationHeatmap;
}

void  City::GenerateCity(int amount){
    // Intersections please, my friend
    for (int i = 0; i < amount; i++){
        roads.push_back(new RoadSegment(roadModel, Vector3{(float)(0 + i), 0, 0}));
    }
}

void City::UpdatePlaneTexture(){
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = populationHeatmap;
}

Texture2D City::GetPopulationHeatmap()const{
    return populationHeatmap;
}

Model City::GetPlane() const{
    return plane;
}