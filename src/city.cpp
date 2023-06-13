#include "include/city.hpp"
#include <iostream>

#define PRINT(x) std::cout << x << std::endl

City::City(float sizeX, float sizeY):
    sizeX(sizeX),
    sizeY(sizeY){
    plane = LoadModelFromMesh(GenMeshPlane(sizeX, sizeY, 3, 3));

    Image heatmap = GenImageColor(sizeX, sizeY, WHITE);
    populationHeatmap = LoadTextureFromImage(heatmap);
    UnloadImage(heatmap);
    UpdatePlaneTexture();
    
}

City::~City(){
    PRINT("Unloading City stuff");
    UnloadTexture(populationHeatmap);
    UnloadModel(plane);

}

Texture2D City::GeneratePopulationHeatmap(int offsetX, int offsetY, float scale){

    Image populationHeatmapImage = GenImagePerlinNoise(sizeX, sizeY, offsetX, offsetY, scale);
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