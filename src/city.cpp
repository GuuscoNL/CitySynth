#include <iostream>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <limits>
#include "include/city.hpp"
#include "raymath.h"
#include "include/highway.hpp"
#include "include/roadsegmentgreaterthan.hpp"
#include "include/simplexnoise.hpp"


#define PRINT(x) std::cout << x << std::endl
#define PRINTVEC2(vec) std::cout << vec.x << ", " << vec.y<< std::endl
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
    for (auto* node : nodes) {
        node->Draw();
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
    DrawCylinder(Vector3{ 0, 0, 0 }, 0.3, 0.3, 0.2, 10, GRAY);
}

Texture2D City::GeneratePopulationHeatmap(int offsetX, int offsetY, float scale) {

    UnloadImage(populationHeatmapImg);
    populationHeatmapImg = GenImageColor(size, size, WHITE);
    SimplexNoise simplexNoise = SimplexNoise(
        settings->frequency,
        settings->amplitude,
        settings->lacunarity,
        settings->persistence);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int noise = round(Remap(simplexNoise.fractal(settings->octaves, i, j), -1, 1, 0, 255));
            Color noiseColor = Color{static_cast<unsigned char>(noise),
                                    static_cast<unsigned char>(noise),
                                    static_cast<unsigned char>(noise),
                                    255 };
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

    Node* startNode = new Node(Vector2{ 0, 0 }, settings);
    Node* nodeL = new Node(Vector2{ settings->highwayLength, 0 }, settings);
    Node* nodeR = new Node(Vector2{ -settings->highwayLength, 0 }, settings);
    nodes.push_back(startNode);
    nodes.push_back(nodeL);
    nodes.push_back(nodeR);
    RoadSegment* roadL = new RoadSegment(0, settings, startNode, nodeL);
    RoadSegment* roadR = new RoadSegment(0, settings, startNode, nodeR);

    Q.push(roadL);
    Q.push(roadR);

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
            Node* nodeToRemove = minRoad->GetTo();
            nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove));

            delete nodeToRemove;
            delete minRoad;
        }
    }
    // Make sure every non used road gets deleted
    while (!Q.empty()) {
        RoadSegment* roadToRemove = Q.top();
        Node* nodeToRemove = roadToRemove->GetTo();
        nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove));
        delete nodeToRemove;
        delete roadToRemove;
        Q.pop();
    }
    nodes.shrink_to_fit();
    PRINT("size: " << roads.size());
    PRINT("size: " << nodes.size());
}

bool City::LocalConstraints(RoadSegment* orgRoad) {
    // Make sure it is in the city
    if (orgRoad->GetToPos().x >= size / 2 - (settings->highwayLength + 1) ||
        orgRoad->GetToPos().x <= -(size / 2) + (settings->highwayLength + 1) ||
        orgRoad->GetToPos().y >= size / 2 - (settings->highwayLength + 1) ||
        orgRoad->GetToPos().y <= -(size / 2) + (settings->highwayLength + 1)) {
        PRINT("REJECTED: OUT OF BOUNDS");
        return false;
    }

    {
        // if “ends close to an existing crossing” then “extend street, to reach the crossing”.
        Node* closestNode;
        Vector2 endNodePos = orgRoad->GetToPos();
        float smallestDistance = std::numeric_limits<float>::max();
        for (auto* node : nodes) {
            float distance = Vector2Distance(endNodePos, node->GetPos());
            if (distance < smallestDistance && node != orgRoad->GetTo()) {
                smallestDistance = distance;
                closestNode = node;
            }
        }

        if (smallestDistance < settings->highwayCloseCrossing) {
            Node* nodeToRemove = orgRoad->GetTo();
            nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove));
            delete nodeToRemove;
            orgRoad->SetTo(closestNode);
            closestNode->color = RED;
            PRINT("CLOSE NODE");
            return true;
        }
    }
    {// Check if roads collide
        for (auto* road : roads) {
            Node* fromNode = road->GetFrom();
            Node* toNode = road->GetTo();
            std::vector<RoadSegment*> ConnectedRoads = orgRoad->GetFrom()->GetConnectedRoads();
            std::vector<RoadSegment*> ConnectedRoadsTo = orgRoad->GetTo()->GetConnectedRoads();
            ConnectedRoads.insert(ConnectedRoads.end(), ConnectedRoadsTo.begin(), ConnectedRoadsTo.end());

            // Don't check roads that are connected to this road, since they will 100% be intersecting.
            if (std::find(ConnectedRoads.begin(), ConnectedRoads.end(), road) != ConnectedRoads.end()) {
                continue;
            }

            Vector2 intersectionPos;

            if (RoadsCollide(road, orgRoad, intersectionPos)) {

                // remove old road
                roads.erase(remove(roads.begin(), roads.end(), road));
                fromNode->RemoveRoad(road);
                toNode->RemoveRoad(road);
                delete road;

                // Remove old Node
                Node* orgToNode = orgRoad->GetTo();
                nodes.erase(remove(nodes.begin(), nodes.end(), orgToNode));
                delete orgToNode;

                // Create new intersection node
                Node* intersectionNode = new Node(intersectionPos, settings);
                nodes.push_back(intersectionNode);

                // Connect all roads to the intersect node
                orgRoad->SetTo(intersectionNode);
                roads.push_back(new RoadSegment(1, settings, fromNode, intersectionNode));
                roads.push_back(new RoadSegment(1, settings, intersectionNode, toNode));
                intersectionNode->color = GREEN;
                PRINT("INTERSECT NODE");
                return true;
            }
        }
    }

    return true;
}

std::vector<RoadSegment*> City::GlobalGoals(RoadSegment* rootRoad) {
    std::vector<RoadSegment*> newRoads;
    Node* newFromNode = rootRoad->GetTo();

    // If it is already split don't add new roads
    if (newFromNode->GetSize() >= 2) {
        return newRoads;
    }

    // Highways
    Vector2 newToPos = HighwaySamples(rootRoad->GetToPos(), rootRoad->GetAngle(),
        settings->highwayAngle);
    Node* toNode = new Node(newToPos, settings);
    nodes.push_back(toNode);
    newRoads.push_back(new RoadSegment(1, settings, newFromNode, toNode));

    if (GetRandomValue(0, 100) <= settings->highwayBranchChange) {
        float angle = 90;
        if (GetRandomValue(0, 1) == 0) {
            angle = -90;
        }
        Node* branchNode = new Node(GetPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->highwayLength), settings);
        nodes.push_back(branchNode);

        newRoads.push_back(new RoadSegment(1, settings, newFromNode, branchNode));
    }
    return newRoads;
}

Vector2 City::GetPosWithAngle(Vector2 fromPos, float angle, float length) {
    float angleRad = angle * DEG2RAD;

    return Vector2{ fromPos.x + cos(angleRad) * length,
                    fromPos.y + sin(angleRad) * length };
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
    for (unsigned int i = 0; i < positionPopulations.size(); i++) {
        if (positionPopulations[i] == maxPop) {
            result = positions[i];
        }
    }
    return result;
}

float City::CrossProduct(Vector2 v1, Vector2 v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

bool City::RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection) {
    Vector2 p1 = road1->GetFromPos();
    Vector2 p2 = road1->GetToPos();
    // PRINT("P1 " << p1.x << ", " << p1.y);
    // PRINT("P2 " << p2.x << ", " << p2.y);

    Vector2 q1 = road2->GetFromPos();
    Vector2 q2 = road2->GetToPos();
    // PRINT("q1 " << q1.x << ", " << q1.y);
    // PRINT("q2 " << q2.x << ", " << q2.y);
    // PRINT("");

    Vector2 d1 = { p2.x - p1.x, p2.y - p1.y };
    Vector2 d2 = { q2.x - q1.x, q2.y - q1.y };

    float d1d2Cross = CrossProduct(d1, d2);
    if (std::abs(d1d2Cross) < std::numeric_limits<float>::epsilon()) {
        return false;  // Lines are parallel or coincident
    }

    Vector2 p1q1 = { q1.x - p1.x, q1.y - p1.y };

    float t = CrossProduct(p1q1, d2) / d1d2Cross;
    float u = CrossProduct(p1q1, d1) / d1d2Cross;

    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        intersection.x = p1.x + t * d1.x;
        intersection.y = p1.y + t * d1.y;
        return true;
    }

    return false;
}


int City::GetPopulationFromHeatmap(Vector2 pos) const {
    Vector2 texPos = Vector2{ heatmapCenter.x + round(pos.y),
                                heatmapCenter.y + round(pos.x) };

    if (!(texPos.x < size && texPos.x > 0)) {
        PRINT("x: " << texPos.x);

    };
    if (!(texPos.y < size && texPos.y > 0)) {
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

    for (auto* node : nodes) {
        delete node;
    }
    nodes.clear();
    nodes.resize(0);
}

Texture2D City::GetPopulationHeatmap() const {
    return populationHeatmapTex;
}

Model City::GetPlane() const {
    return plane;
}