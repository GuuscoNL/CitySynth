#include <iostream>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "include/city.hpp"
#include "raymath.h"
#include "include/highway.hpp"
#include "include/roadsegmentgreaterthan.hpp"


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

City::City(float size, Shader shader) :
    size(size),
    shader(shader) {
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));
    plane.materials[0].shader = shader;

    populationHeatmap = LoadTextureFromImage(GenImageColor(size, size, WHITE));
    heatmapCenter = Vector2{ round(size / 2), round(size / 2) };

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
    Q.push(new RoadSegment(0, shader, Vector2{ 0, 0 }, Vector2{ 2, 0 }));

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
    if (road->GetToPos().x > size / 2 - 3 || road->GetToPos().x < -(size / 2) + 3 ||
        road->GetToPos().y > size / 2 - 3 || road->GetToPos().y < -(size / 2) + 3) {
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
                                        30);
    newRoads.push_back(new RoadSegment(1, shader, newFromPos, newToPos));
    return newRoads;
}

Vector2 City::GetPosWithAngle(Vector2 fromPos, float angle) {
    float angleRad = angle * DEG2RAD;
    
    
    return Vector2{ fromPos.x + cos(angleRad) * 3,
                    fromPos.y + sin(angleRad) * 3 };
}

Vector2 City::HighwaySamples(Vector2 fromPos, float OriginalAngle, float MaxAngle) {
    std::vector<Vector2> positions;
    for (int i = 0; i < 15; i++) {
        positions.push_back(GetPosWithAngle(fromPos, GetRandomValue(-MaxAngle + OriginalAngle, MaxAngle + OriginalAngle)));
    }

    std::vector<int> positionPopulations;
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
    Image heatmapImage = LoadImageFromTexture(populationHeatmap);
    // PRINT(pos.x);
    // PRINT(pos.y);

    if (! (texPos.x < size && texPos.x > 0)){
        PRINT("x: " << texPos.x);

    };
    if (! (texPos.y < size && texPos.y > 0)){
        PRINT("y: " << texPos.y);
    };
    Color PopulationColor = GetImageColor(heatmapImage, texPos.x, texPos.y);
    UnloadImage(heatmapImage);
    return PopulationColor.r;
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