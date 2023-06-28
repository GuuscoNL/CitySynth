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

City::City(float size, Settings* settings) :
    size(size),
    settings(settings) {

    // temp models
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));
    plane.materials[0].shader = settings->shader;

    // temp Image
    populationHeatmapImg = GenImageColor(size, size, WHITE);
    populationHeatmapTex = LoadTextureFromImage(populationHeatmapImg);
    heatmapCenter = Vector2{ round(size / 2), round(size / 2) };

    SetSize(size);
}

City::~City() {
    UnloadTexture(populationHeatmapTex);
    UnloadImage(populationHeatmapImg);
    UnloadModel(plane);

    ResetCity();
}

void City::SetSize(int size) {
    this->size = size;

    // Generate plane
    plane = LoadModelFromMesh(GenMeshPlane(size, size, 3, 3));
    plane.materials[0].shader = settings->shader;

    // Update heatmap based on new size
    if (settings->useCustomHeatmap) {
        SetHeatmap(settings->customHeatmap);
    }
    else {
        GeneratePopulationHeatmap();
    }

    heatmapCenter = Vector2{ (float)round(size / 2), (float)round(size / 2) };

    UpdatePlaneTexture();
}

void City::Draw() const {

    for (const auto* const road : roads) {
        road->Draw();
    }

    if (settings->ShowNodes) {
        for (const auto* const node : nodes) {
            node->Draw();
        }
    }

    DrawModel(plane, Vector3Zero(), 1.0f, WHITE);
}

void City::GeneratePopulationHeatmap() {

    Image heatmap = GenImageColor(size, size, WHITE);

    SimplexNoise simplexNoise = SimplexNoise(
        settings->frequency,
        settings->amplitude,
        settings->lacunarity,
        settings->persistence);

    // Fill image with Simplexnoise
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int noise = round(Remap(simplexNoise.fractal(settings->octaves, i, j), -1, 1, 0, 255));
            Color noiseColor = Color{ static_cast<unsigned char>(noise),
                                    static_cast<unsigned char>(noise),
                                    static_cast<unsigned char>(noise),
                                    255 };
            ImageDrawPixel(&heatmap, i, j, noiseColor);
        }
    }

    SetHeatmap(heatmap);
}

void City::SetHeatmap(Image heatmapImage) {

    UnloadImage(populationHeatmapImg);
    Image heatmapCopy = ImageCopy(heatmapImage);

    ImageResize(&heatmapCopy, size, size);
    populationHeatmapImg = heatmapCopy;

    UnloadTexture(populationHeatmapTex);
    populationHeatmapTex = LoadTextureFromImage(heatmapCopy);

    UpdatePlaneTexture();
}

void  City::GenerateCity(unsigned int segmentLimit) {
    // Make sure the city is empty
    ResetCity();

    roads.reserve(segmentLimit); // speed!
    nodes.reserve(segmentLimit / 2); // speed!
    std::priority_queue<RoadSegment*, std::vector<RoadSegment*>, RoadSegmentGreaterThan> Q;

    // Make starting nodes
    Node* startNode = new Node(Vector2{ 0, 0 }, settings);
    Node* nodeL = new Node(Vector2{ settings->highwayLength, 0 }, settings);
    Node* nodeR = new Node(Vector2{ -settings->highwayLength, 0 }, settings);

    nodes.push_back(startNode);
    nodes.push_back(nodeL);
    nodes.push_back(nodeR);

    // Make starting roads
    RoadSegment* roadL = new Highway(0, settings, startNode, nodeL);
    RoadSegment* roadR = new Highway(0, settings, startNode, nodeR);

    Q.push(roadL);
    Q.push(roadR);

    while (!Q.empty() && roads.size() < segmentLimit) {
        RoadSegment* minRoad = Q.top();
        Q.pop();
        // Check if it intersects ir goes out of bounds.
        bool accepted = LocalConstraints(minRoad);

        if (accepted) {
            roads.push_back(minRoad);
            std::vector<RoadSegment*> newRoads;

            // Will this road create more roads?
            GlobalGoals(minRoad, newRoads);

            // Add the new possible roads to the queue with delay
            for (auto* newRoad : newRoads) {
                newRoad->SetDelay(minRoad->GetDelay() + 1 + newRoad->GetDelay());
                Q.push(newRoad);
            }
        }
        else {
            // Road has been rejected, remove it
            Node* nodeToRemove = minRoad->GetTo();
            roads.erase(remove(roads.begin(), roads.end(), minRoad), roads.end());

            delete minRoad;
            DeleteNode(nodeToRemove);
        }
    }
    // Make sure every non used road gets deleted
    while (!Q.empty()) {
        RoadSegment* roadToRemove = Q.top();
        Node* nodeToRemove = roadToRemove->GetTo();
        roads.erase(remove(roads.begin(), roads.end(), roadToRemove), roads.end());
        delete roadToRemove;

        DeleteNode(nodeToRemove);
        Q.pop();
    }
    // Keep memory minimal
    nodes.shrink_to_fit();
    roads.shrink_to_fit();

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

    // if “two streets intersect” then “generate a crossing”.
    if (LocalConstraintIntersecting(orgRoad)) {
        return true;
    }
    
    // if “ends close to an existing crossing” then “extend street, to reach the crossing”        
    if (LocalConstraintCloseCrossing(orgRoad)) {
        return true;
    }

    // if “close to intersecting” then “extend street to form intersection”
    if (LocalConstraintCloseRoad(orgRoad)) {
        return true;
    }

    return true;
}

bool City::LocalConstraintIntersecting(RoadSegment* orgRoad) {
    for (auto* road : roads) {
        std::vector<RoadSegment*> ConnectedRoads = orgRoad->GetFrom()->GetConnectedRoads();
        std::vector<RoadSegment*> ConnectedRoadsTo = orgRoad->GetTo()->GetConnectedRoads();
        ConnectedRoads.insert(ConnectedRoads.end(), ConnectedRoadsTo.begin(), ConnectedRoadsTo.end());

        // Don't check roads that are connected to this road, since they will always be intersecting.
        if (std::find(ConnectedRoads.begin(), ConnectedRoads.end(), road) != ConnectedRoads.end()) {
            continue;
        }

        Vector2 intersectionPos;

        // If the roads collide make an intersection
        if (RoadsCollide(road, orgRoad, intersectionPos)) {

            Node* intersectionNode = AddIntersection(road, orgRoad, intersectionPos);
            intersectionNode->color = GREEN;
            return true;
        }
    }
    return false;
}

bool City::LocalConstraintCloseCrossing(RoadSegment* orgRoad) {
    Node* closestNode = nullptr;

    Vector2 endNodePos = orgRoad->GetToPos();
    float smallestDistance = std::numeric_limits<float>::max();

    // Loop over all nodes and and check which one is the closest
    for (auto* node : nodes) {
        // Distance in sqr
        float distance = Vector2DistanceSqr(endNodePos, node->GetPos());
        if (distance < smallestDistance && node != orgRoad->GetTo()) {
            smallestDistance = distance;
            closestNode = node;
        }
    }

    // If the closest node is closer than `CloseCrossing` then extend street to reach the crossing
    if (smallestDistance < settings->CloseCrossing * settings->CloseCrossing && closestNode != nullptr) {

        Node* nodeToRemove = orgRoad->GetTo();
        orgRoad->SetTo(closestNode);
        closestNode->color = RED;

        // Remove old node
        DeleteNode(nodeToRemove);
        return true;
    }
    return false;
}

bool City::LocalConstraintCloseRoad(RoadSegment* orgRoad) {
    Node* orgToNode = orgRoad->GetTo();
    Vector2 closestPoint;
    Vector2 closestClosestPoint;
    RoadSegment* closestRoad = nullptr;
    float smallestDistance = std::numeric_limits<float>::max();

    // Loop over all roads to get the closest road
    for (auto* road : roads) {
        // Distance in sqr
        float distance = DistNodeToRoad(orgToNode, road, closestPoint);
        if (distance < smallestDistance && road != orgRoad) {
            smallestDistance = distance;
            closestRoad = road;
            closestClosestPoint = closestPoint;
        }
    }

    //  Make an intersection if the two roads are close enough
    if (smallestDistance < settings->CloseRoad * settings->CloseRoad && closestRoad != nullptr) {
        Node* intersectionNode = AddIntersection(closestRoad, orgRoad, closestClosestPoint);
        intersectionNode->color = BLUE;
        return true;
    }
    return false;
}

void City::GlobalGoals(RoadSegment* rootRoad, std::vector<RoadSegment*>& newRoads) {

    // If it is already split don't add new roads, because there was already a conflict resolved
    if (rootRoad->GetTo()->GetAmountConnectedRoads() >= 2) {
        return;
    }

    // Highways
    if (rootRoad->GetType() == RoadSegment::HIGHWAY) {
        // First get the next straight Highway road
        // Use HighwaySamples to get the next node where the pop is the highest
        Vector2 newToPos = HighwaySamples(rootRoad, settings->highwayAngle);
        Node* toNode = new Node(newToPos, settings);
        nodes.push_back(toNode);
        newRoads.emplace_back( new Highway(1, settings, rootRoad->GetTo(), toNode));

        // Will this highway branch to new highways?
        if (GetRandomValue(0, 10000) / 100.f <= settings->highwayBranchChance) {
            // Random angle 90 or -90
            float angle = 90;
            if (GetRandomValue(0, 1) == 0) {
                angle = -90;
            }
            Node* branchNode = new Node(CalcPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->highwayLength), settings);
            nodes.push_back(branchNode);

            newRoads.emplace_back( new Highway(1, settings, rootRoad->GetTo(), branchNode));

        }
        // Will this road branch to a new Side road?
        else if (GetRandomValue(0, 10000) / 100.f <= settings->highwaySideRoadBranchChance) {
            // Random angle 90 or -90
            float angle = 90;
            if (GetRandomValue(0, 1) == 0) {
                angle = -90;
            }
            AddSideRoad(rootRoad, angle, newRoads);
        }
    }
    // Side roads
    else if (rootRoad->GetType() == RoadSegment::SIDEROAD) {
        // First get the next straight Side road 
        AddSideRoad(rootRoad, 0, newRoads);

        // Will this Side road branch to new Side road?
        if (GetRandomValue(0, 10000) / 100.f <= settings->sideRoadBranchChance) {
            AddSideRoad(rootRoad, 90, newRoads);
        }
    }
}

void City::DeleteNode(Node* nodeToRemove) {
    // Make sure the node is not used by other roads before deleting
    if (nodeToRemove->GetAmountConnectedRoads() <= 0) {
        nodes.erase(remove(nodes.begin(), nodes.end(), nodeToRemove), nodes.end());
        delete nodeToRemove;
    }
}

void City::AddSideRoad(RoadSegment* rootRoad, float angle, std::vector<RoadSegment*>& newRoads) {
    Vector2 newPos = CalcPosWithAngle(rootRoad->GetToPos(), rootRoad->GetAngle() + angle, settings->sideRoadLength);

    // Check if the pop is high enough for a new Side Road
    if (GetPopulationFromHeatmap(newPos) / 255. >= settings->sideRoadThreshold) {
        Node* newToNode = new Node(newPos, settings);
        nodes.push_back(newToNode);
        newRoads.emplace_back( new SideRoad(1, settings, rootRoad->GetTo(), newToNode));
    }
}

Vector2 City::CalcPosWithAngle(const Vector2& fromPos, float angle, float length) const {
    float angleRad = angle * DEG2RAD;

    return Vector2{ fromPos.x + cos(angleRad) * length,
                    fromPos.y + sin(angleRad) * length };
}

Vector2 City::HighwaySamples(const RoadSegment* const rootRoad, float MaxAngle) const {
    std::vector<Vector2> positions;
    positions.reserve(settings->highwaySampleAmount); // Speed!

    //  Get positions with random angles
    for (int i = 0; i < settings->highwaySampleAmount; i++) {
        positions.emplace_back(CalcPosWithAngle(rootRoad->GetToPos(), GetRandomValue(-MaxAngle + rootRoad->GetAngle(), MaxAngle + rootRoad->GetAngle()), settings->highwayLength));
    }

    // Check if the values of the pop for every postion
    std::vector<int> positionPopulations;
    positionPopulations.reserve(positions.size());
    for (const Vector2& pos : positions) {
        positionPopulations.emplace_back(GetPopulationFromHeatmap(pos));
    }
    // Get highest pop
    int maxPop = *std::max_element(positionPopulations.begin(), positionPopulations.end());
    Vector2 result;
    // Get the pos that has the highest pop
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

    // if the intersection pos is close to another road don't create an intersection,
    // but extend road to the close node. This prevents extremely small roads.
    if (Vector2DistanceSqr(fromNode->GetPos(), intersectionPos) < 0.2 * 0.2) {
        toAddRoad->SetTo(fromNode);

        DeleteNode(orgToNode);
        return fromNode;
    }

    if (Vector2DistanceSqr(toNode->GetPos(), intersectionPos) < 0.2 * 0.2) {
        toAddRoad->SetTo(toNode);

        DeleteNode(orgToNode);
        return toNode;
    }

    // Create new intersection node
    Node* intersectionNode = new Node(intersectionPos, settings);
    nodes.push_back(intersectionNode);

    // Connect all roads to the intersect node and use the correct road type
    toAddRoad->SetTo(intersectionNode);
    if (toSplitRoad->GetType() == RoadSegment::HIGHWAY) {
        roads.emplace_back(new Highway(1, settings, fromNode, intersectionNode));
        roads.emplace_back(new Highway(1, settings, intersectionNode, toNode));

    }
    else if (toSplitRoad->GetType() == RoadSegment::SIDEROAD) {
        roads.emplace_back(new SideRoad(1, settings, fromNode, intersectionNode));
        roads.emplace_back(new SideRoad(1, settings, intersectionNode, toNode));

    }
    else {
        PRINT("AddIntersection(): Unknown road type!");
    }

    // remove old road
    roads.erase(remove(roads.begin(), roads.end(), toSplitRoad), roads.end());

    delete toSplitRoad;
    DeleteNode(orgToNode);

    return intersectionNode;
}

float City::CrossProduct(const Vector2& v1, const Vector2& v2) const {
    return v1.x * v2.y - v1.y * v2.x;
}

// This is done with the help of ChatGPT, I couldn't find any easy way anywhere else.
bool City::RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection) const {
    Vector2 p1 = road1->GetFromPos();
    Vector2 p2 = road1->GetToPos();

    Vector2 q1 = road2->GetFromPos();
    Vector2 q2 = road2->GetToPos();

    // Calculate the direction vectors
    Vector2 d1 = { p2.x - p1.x, p2.y - p1.y };
    Vector2 d2 = { q2.x - q1.x, q2.y - q1.y };

    float d1d2Cross = CrossProduct(d1, d2);

    // Check if the cross product is close to zero, indicating parallel or coincident lines
    if (std::abs(d1d2Cross) < std::numeric_limits<float>::epsilon()) {
        return false;
    }

    Vector2 p1q1 = { q1.x - p1.x, q1.y - p1.y };

    // Calculate the parameters t and u for intersection calculation
    float t = CrossProduct(p1q1, d2) / d1d2Cross;
    float u = CrossProduct(p1q1, d1) / d1d2Cross;

    // Check if the intersection point lies within the line segments of both roads
    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        intersection.x = p1.x + t * d1.x;
        intersection.y = p1.y + t * d1.y;
        return true;
    }

    return false;
}

// This algorithm was based on the algorithm explained in this video:
// https://youtu.be/egmZJU-1zPU - Two-Bit Coding
float City::DistNodeToRoad(Node* node, RoadSegment* road, Vector2& closestPoint) const {
    Vector2 p = node->GetPos();
    Vector2 a = road->GetFromPos();
    Vector2 b = road->GetToPos();

    Vector2 ab = Vector2Subtract(b, a);
    Vector2 ap = Vector2Subtract(p, a);

    // Calculate projection
    float proj = Vector2DotProduct(ap, ab);
    float abLenSqr = Vector2LengthSqr(ab);
    float d = proj / abLenSqr; // Normalize projection

    if (d <= 0) {
        closestPoint = a; // Closest point is the From node
    }
    else if (d >= 1) {
        closestPoint = b; // Closest point is the To node
    }
    else {
        closestPoint = Vector2Add(a, Vector2Scale(ab, d));
    }

    return Vector2DistanceSqr(p, closestPoint);
}

int City::GetPopulationFromHeatmap(const Vector2& pos) const {
    // Translate pos to a pos on the texture
    Vector2 texPos = Vector2{ heatmapCenter.x + round(pos.y),
                                heatmapCenter.y + round(pos.x) };

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

    // Reset next ID
    if (nodes.size() > 0) {
        nodes[0]->ResetId();
    }
    for (auto* node : nodes) {
        delete node;
    }
    nodes.clear();
    nodes.resize(0);
}

std::vector<RoadSegment*> City::GetRoads() const {
    return roads;
}

std::vector<Node*> City::GetNodes() const {
    return nodes;
}

int City::GetRoadsSize() const {
    return roads.size();
}

int City::GetNodesSize() const {
    return nodes.size();
}