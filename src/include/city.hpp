#pragma once
#include <raylib.h>

class City{
    public:
    City(float sizeX, float sizeY);
    ~City();
    Texture2D GeneratePopulationHeatmap(int offsetX=0, int offsetY=0, float scale=1.0f);
    Texture2D GetPopulationHeatmap();
    Model GetPlane();

    private:
    float sizeX;
    float sizeY;
    Texture2D populationHeatmap;
    Model plane;

    void UpdatePlaneTexture();

};