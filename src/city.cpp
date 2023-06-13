#include <iostream>
#include "include/city.hpp"
#include "raymath.h"

#define PRINT(x) std::cout << x << std::endl

City::City(float size):
    size(size){
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

}

void City::Draw(){
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

void City::UpdatePlaneTexture(){
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = populationHeatmap;
}

Texture2D City::GetPopulationHeatmap(){
    return populationHeatmap;
}

Model City::GetPlane(){
    return plane;
}