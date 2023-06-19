#include <iostream>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "include/city.hpp"
#include "raymath.h"
#include "include/highway.hpp"
#include "include/roadsegmentgreaterthan.hpp"
#include "include/simplexnoise.hpp"



#define PRINT(x) std::cout << x << std::endl
#define PRINT_COLOR_RGB(color) \
    do { \
        int r = (color).r; \
        int g = (color).g; \
        int b = (color).b; \
        std::cout << "R: " << r << std::endl; \
        std::cout << "G: " << g << std::endl; \
        std::cout << "B: " << b << std::endl; \
    } while (false)

City::City(float size, Settings* settings) :
    size(size),
    settings(settings) {
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));
    plane.materials[0].shader = settings->shader;

    populationHeatmapImg = GenImageColor(size, size, WHITE);
    populationHeatmapTex = LoadTextureFromImage(populationHeatmapImg);
    heatmapCenter = Vector2{ round(size / 2), round(size / 2) };

    UpdatePlaneTexture();
}

City::~City() {
    PRINT("Unloading City");
    UnloadTexture(populationHeatmapTex);
    UnloadImage(populationHeatmapImg);
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

    UnloadImage(populationHeatmapImg);
    populationHeatmapImg = GenImageColor(size, size, WHITE);
    SimplexNoise simplexNoise = SimplexNoise(settings->frequency, 
                                            settings->amplitude, 
                                            settings->lacunarity, 
                                            settings->persistence);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int noise = round(Remap(simplexNoise.fractal(settings->octaves, i, j), -1, 1, 0, 255));
            Color noiseColor = Color{   static_cast<unsigned char>(noise), 
                                        static_cast<unsigned char>(noise), 
                                        static_cast<unsigned char>(noise),
                                        255};
            ImageDrawPixel(&populationHeatmapImg, i, j, noiseColor);
        }
    }
    UnloadTexture(populationHeatmapTex);
    populationHeatmapTex = LoadTextureFromImage(populationHeatmapImg);
    
    UpdatePlaneTexture();
    return populationHeatmapTex;
}

void  City::GenerateCity(unsigned int amount) {
    ResetCity();
    roads.reserve(amount); // speed!
    std::priority_queue<RoadSegment*, std::vector<RoadSegment*>, RoadSegmentGreaterThan> Q;
    Q.push(new RoadSegment(0, settings, Vector2{ 0, 0 }, Vector2{ settings->highwayLength, 0 }));
    Q.push(new RoadSegment(0, settings, Vector2{ 0, 0 }, Vector2{ -settings->highwayLength, 0 }));

    while (!Q.empty() && roads.size() < amount) {
        RoadSegment* minRoad = Q.top();
        Q.pop();
        bool accepted = LocalConstraints(minRoad);
        if (accepted) {
            roads.push_back(minRoad);
            // PRINT(*minRoad);
            std::vector<RoadSegment*> newRoads = GlobalGoals(minRoad);
            for (auto* newRoad : newRoads) {
                newRoad->SetDelay(minRoad->GetDelay() + 1 + newRoad->GetDelay());
                Q.push(newRoad);
            }
        }
        else {
            delete minRoad;
        }
    }

    // Make sure every non used roads get deleted
    while (!Q.empty()) {
        delete Q.top();
        Q.pop();
    }
    PRINT("size: " << roads.size());
}

bool City::LocalConstraints(RoadSegment* road) {
    // Make sure it is in the city
    if (road->GetToPos().x >= size / 2 - settings->highwayLength || road->GetToPos().x <= -(size / 2) + settings->highwayLength ||
        road->GetToPos().y >= size / 2 - settings->highwayLength || road->GetToPos().y <= -(size / 2) + settings->highwayLength) {
            PRINT("REJECTED!");
            return false;
        }
    return true;
}

std::vector<RoadSegment*> City::GlobalGoals(RoadSegment* rootRoad) {
    std::vector<RoadSegment*> newRoads;
    Vector2	newFromPos = rootRoad->GetToPos();
    
    // Highways
    Vector2 newToPos = HighwaySamples(rootRoad->GetToPos(), rootRoad->GetAngle(),
                                        settings->highwayAngle);
    newRoads.push_back(new RoadSegment(1, settings, newFromPos, newToPos));

    if (GetRandomValue(0, 100) <= settings->highwayBranchChange) {
        float angle = 90;
        if (GetRandomValue(0,1) == 0) {
            angle = -90;
        }

        newRoads.push_back(new RoadSegment(1, settings, newFromPos, 
        GetPosWithAngle(newFromPos, rootRoad->GetAngle() + angle, settings->highwayLength)));
    }
    return newRoads;
}

Vector2 City::GetPosWithAngle(Vector2 fromPos, float angle, float length) {
    float angleRad = angle * DEG2RAD;
    
    return Vector2{ fromPos.x + cos(angleRad) * length,
                    fromPos.y + sin(angleRad) * length};
}

Vector2 City::HighwaySamples(Vector2 fromPos, float OriginalAngle, float MaxAngle) {
    std::vector<Vector2> positions;
    positions.reserve(settings->highwaySampleAmount);

    for (int i = 0; i < settings->highwaySampleAmount; i++) {
        positions.push_back(GetPosWithAngle(fromPos, GetRandomValue(-MaxAngle + OriginalAngle, MaxAngle + OriginalAngle), settings->highwayLength));
    }

    std::vector<int> positionPopulations;
    positionPopulations.reserve(positions.size());
    for (Vector2 pos : positions) {
        positionPopulations.push_back(GetPopulationFromHeatmap(pos));
    }
    int maxPop = *std::max_element(positionPopulations.begin(), positionPopulations.end());
    Vector2 result;
    for (int i = 0; i < positionPopulations.size(); i++)
    {
        if (positionPopulations[i] == maxPop){
            result = positions[i];
        }
    }
    return result;
}

int City::GetPopulationFromHeatmap(Vector2 pos) const {
    Vector2 texPos = Vector2{   heatmapCenter.x + round(pos.y), 
                                heatmapCenter.y + round(pos.x) };

    if (! (texPos.x < size && texPos.x > 0)){
        PRINT("x: " << texPos.x);

    };
    if (! (texPos.y < size && texPos.y > 0)){
        PRINT("y: " << texPos.y);
    };
    Color PopulationColor = GetImageColor(populationHeatmapImg, texPos.x, texPos.y);
    return PopulationColor.r;
}

void City::UpdatePlaneTexture() {
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = populationHeatmapTex;
}

void City::ResetCity() {
    for (auto* road : roads) {
        delete road;
    }
    roads.clear();
    roads.resize(0);
}

Texture2D City::GetPopulationHeatmap() const {
    return populationHeatmapTex;
}

Model City::GetPlane() const {
    return plane;
}