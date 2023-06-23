#include <iostream>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <limits>
#include "include/city.hpp"
#include "raymath.h"
#include "include/highway.hpp"
#include "include/sideroad.hpp"
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

    for (auto*& road : roads) {
        road->Draw();
    }
    if (settings->ShowNodes) {
        for (auto*& node : nodes) {
            node->Draw();
        }
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
    nodes.reserve(amount/2); // speed!
    std::priority_queue<RoadSegment*, std::vector<RoadSegment*>, RoadSegmentGreaterThan> Q;

    Node* startNode = new Node(Vector2{ 0, 0 }, settings);
    Node* nodeL = new Node(Vector2{ settings->highwayLength, 0 }, settings);
    Node* nodeR = new Node(Vector2{ -settings->highwayLength, 0 }, settings);
    nodes.push_back(startNode);
    nodes.push_back(nodeL);
    nodes.push_back(nodeR);
    RoadSegment* roadL = new Highway(0, settings, startNode, nodeL);
    RoadSegment* roadR = new Highway(0, settings, startNode, nodeR);

    Q.push(roadL);
    Q.push(roadR);

    while (!Q.empty() && roads.size() < amount) {
        RoadSegment* minRoad = Q.top();
        Q.pop();
        bool accepted = LocalConstraints(minRoad);

        if (accepted) {
            roads.push_back(minRoad);
            std::vector<RoadSegment*> newRoads = GlobalGoals(minRoad);

            for (auto* newRoad : newRoads) {
                newRoad->SetDelay(minRoad->GetDelay() + 1 + newRoad->GetDelay());
                Q.push(newRoad);
            }
        }
        else {
            Node* nodeToRemove = minRoad->GetTo();
            nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
            roads.erase(remove(roads.begin(), roads.end(), minRoad), roads.end());

            delete minRoad;
            delete nodeToRemove;
        }
    }
    // Make sure every non used road gets deleted
    while (!Q.empty()) {
        RoadSegment* roadToRemove = Q.top();
        Node* nodeToRemove = roadToRemove->GetTo();
        roads.erase(remove(roads.begin(), roads.end(), roadToRemove), roads.end());
        delete roadToRemove;

        // If this node is used by other roads do not delete.
        if (nodeToRemove->GetSize() <= 0) {
            nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
            delete nodeToRemove;
        }
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

    { // Check if roads collide
        for (auto* road : roads) {
            std::vector<RoadSegment*> ConnectedRoads = orgRoad->GetFrom()->GetConnectedRoads();
            std::vector<RoadSegment*> ConnectedRoadsTo = orgRoad->GetTo()->GetConnectedRoads();
            ConnectedRoads.insert(ConnectedRoads.end(), ConnectedRoadsTo.begin(), ConnectedRoadsTo.end());

            // Don't check roads that are connected to this road, since they will 100% be intersecting.
            if (std::find(ConnectedRoads.begin(), ConnectedRoads.end(), road) != ConnectedRoads.end()) {
                continue;
            }

            Vector2 intersectionPos;

            if (RoadsCollide(road, orgRoad, intersectionPos)) {

                Node* intersectionNode = AddIntersection(road, orgRoad, intersectionPos);
                intersectionNode->color = GREEN;
                // PRINT("INTERSECT NODE");
                return true;
            }
        }
    }

    { // if “ends close to an existing crossing” then “extend street, to reach the crossing”.        
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

        if (smallestDistance < settings->CloseCrossing) {
            Node* nodeToRemove = orgRoad->GetTo();
            nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
            orgRoad->SetTo(closestNode);
            closestNode->color = RED;
            // PRINT("CLOSE NODE");
            if (nodeToRemove->GetSize() <= 0) {
                nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
                delete nodeToRemove;
            }
            return true;
        }
    }

    { // if “close to intersecting” then “extend street to form intersection”
        Node* orgToNode = orgRoad->GetTo();
        Vector2 intersectionPos;
        Vector2 closestIntersectionPos;
        RoadSegment* closestRoad;
        float smallestDistance = std::numeric_limits<float>::max();
        for (auto* road : roads) {
            float distance = DistNodeToRoad(orgToNode, road, intersectionPos);
            if ( distance < smallestDistance && road != orgRoad) {
                smallestDistance = distance;
                closestRoad = road;
                closestIntersectionPos = intersectionPos;
            }
        }

        if (smallestDistance < settings->CloseRoad) {
            Node* intersectionNode = AddIntersection(closestRoad, orgRoad, closestIntersectionPos);
            intersectionNode->color = BLUE;
            // PRINT("CLOSE ROAD");
            return true;

        }
    }

    return true;
}

std::vector<RoadSegment*> City::GlobalGoals(RoadSegment* rootRoad) {
    std::vector<RoadSegment*> newRoads;
    Node* newFromNode = rootRoad->GetTo();

    // If it is already split don't add new roads (there was already a conflict resolved)
    if (newFromNode->GetSize() >= 2) {
        return newRoads;
    }

    // Highways
    if (rootRoad->GetType() == RoadSegment::HIGHWAY) {
        Vector2 newToPos = HighwaySamples(rootRoad->GetToPos(), rootRoad->GetAngle(),
            settings->highwayAngle);
        Node* toNode = new Node(newToPos, settings);
        nodes.push_back(toNode);
        newRoads.push_back(new Highway(1, settings, newFromNode, toNode));

        if (GetRandomValue(0, 100) <= settings->highwayBranchChance) {
            // HIGHWAYS
            float angle = 90;
            if (GetRandomValue(0, 1) == 0) {
                angle = -90;
            }
            Node* branchNode = new Node(GetPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->highwayLength), settings);
            nodes.push_back(branchNode);

            newRoads.push_back(new Highway(1, settings, newFromNode, branchNode));
    
        } else if (GetRandomValue(0, 100) <= settings->highwaySideRoadBranchChance) {
            // SIDEROADS
            float angle = 90;
            if (GetRandomValue(0, 1) == 0) {
                angle = -90;
            }
            Vector2 newPos = GetPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->sideRoadLength);
            if (GetPopulationFromHeatmap(newPos) / 255. >= settings->sideRoadThreshold) {
                Node* branchNode = new Node(newPos, settings);
                nodes.push_back(branchNode);
                newRoads.push_back(new SideRoad(settings->sideRoadBranchDelay, settings, newFromNode, branchNode));
            }
        }
    }
    else if (rootRoad->GetType() == RoadSegment::SIDEROAD) {
        {    
            float angle = 0;
            Vector2 newPos = GetPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->sideRoadLength);
            if (GetPopulationFromHeatmap(newPos) / 255. >= settings->sideRoadThreshold) {
                Node* newToNode = new Node(newPos, settings);
                nodes.push_back(newToNode);
                newRoads.push_back(new SideRoad(1, settings, newFromNode, newToNode));
            }
        }
        if (GetRandomValue(0, 100) <= settings->sideRoadBranchChance) {
            float angle = 90;
            // if (GetRandomValue(0, 1) == 0) { // Does not look good
            //     angle = -90;
            // }
            Vector2 newPos = GetPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->sideRoadLength);
            if (GetPopulationFromHeatmap(newPos) / 255. >= settings->sideRoadThreshold) {
                Node* newToNode = new Node(newPos, settings);
                nodes.push_back(newToNode);
                newRoads.push_back(new SideRoad(1, settings, newFromNode, newToNode));
            }
        }
    }
    return newRoads;
}

Vector2 City::GetPosWithAngle(const Vector2& fromPos, float angle, float length) {
    float angleRad = angle * DEG2RAD;

    return Vector2{ fromPos.x + cos(angleRad) * length,
                    fromPos.y + sin(angleRad) * length };
}

Vector2 City::HighwaySamples(const Vector2& fromPos, float OriginalAngle, float MaxAngle) {
    std::vector<Vector2> positions;
    positions.reserve(settings->highwaySampleAmount);

    for (int i = 0; i < settings->highwaySampleAmount; i++) {
        positions.push_back(GetPosWithAngle(fromPos, GetRandomValue(-MaxAngle + OriginalAngle, MaxAngle + OriginalAngle), settings->highwayLength));
    }

    std::vector<int> positionPopulations;
    positionPopulations.reserve(positions.size());
    for (const Vector2& pos : positions) {
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

Node* City::AddIntersection(RoadSegment* toSplitRoad, RoadSegment* toAddRoad, const Vector2& intersectionPos) {
        Node* fromNode = toSplitRoad->GetFrom();
        Node* toNode = toSplitRoad->GetTo();

        // Remove old Node
        Node* orgToNode = toAddRoad->GetTo();
        nodes.erase(remove(nodes.begin(), nodes.end(), orgToNode), nodes.end());

        if (Vector2Distance(fromNode->GetPos(), intersectionPos) < 0.2) {
            toAddRoad->SetTo(fromNode);
            return fromNode;
        }

        if (Vector2Distance(toNode->GetPos(), intersectionPos) < 0.2) {
            toAddRoad->SetTo(toNode);
            return toNode;
        }

        // Create new intersection node
        Node* intersectionNode = new Node(intersectionPos, settings);
        nodes.push_back(intersectionNode);

        // Connect all roads to the intersect node
        toAddRoad->SetTo(intersectionNode);
        if (toSplitRoad->GetType() == RoadSegment::HIGHWAY) {
            roads.push_back(new Highway(1, settings, fromNode, intersectionNode));
            roads.push_back(new Highway(1, settings, intersectionNode, toNode));

        } else if (toSplitRoad->GetType() == RoadSegment::SIDEROAD) {
            roads.push_back(new SideRoad(1, settings, fromNode, intersectionNode));
            roads.push_back(new SideRoad(1, settings, intersectionNode, toNode));

        } else {
            PRINT("AddIntersection(): Unknown type!");
        }

        // remove old road
        roads.erase(remove(roads.begin(), roads.end(), toSplitRoad), roads.end());

        delete toSplitRoad;
        if (orgToNode->GetSize() <= 0) {
            nodes.erase(remove(nodes.begin(), nodes.end(), orgToNode), nodes.end());
            delete orgToNode;
        }

        return intersectionNode;
}

float City::CrossProduct(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

bool City::RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection) {
    Vector2 p1 = road1->GetFromPos();
    Vector2 p2 = road1->GetToPos();

    Vector2 q1 = road2->GetFromPos();
    Vector2 q2 = road2->GetToPos();

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

// https://youtu.be/egmZJU-1zPU
float City::DistNodeToRoad(Node* node, RoadSegment* road, Vector2& intersection) {
    Vector2 p = node->GetPos();
    Vector2 a = road->GetFromPos();
    Vector2 b = road->GetToPos();

    Vector2 ab = Vector2Subtract(b, a);
    Vector2 ap = Vector2Subtract(p, a);


    float proj = Vector2DotProduct(ap, ab);
    float abLenSqr = Vector2LengthSqr(ab);
    float d = proj / abLenSqr;

    if ( d <= 0) {
        intersection = a;
    }
    else if (d >= 1) {
        intersection = b;
    }
    else {
        intersection = Vector2Add(a, Vector2Scale(ab, d));
    }

    return Vector2Distance(p, intersection);

}

int City::GetPopulationFromHeatmap(const Vector2& pos) const {
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

std::vector<RoadSegment*> City::GetRoads() {
    return roads;
}