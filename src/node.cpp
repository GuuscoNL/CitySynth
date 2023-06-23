
#include <iostream>
#include "include/node.hpp"
#include <algorithm>

#define PRINT(x) std::cout << x << std::endl

Node::Node(const Vector2& pos2D, Settings* settings) :
    pos2D(pos2D),
    settings(settings) {
    model = settings->NodeModel;
    model.materials[0].shader = settings->shader;
    color = WHITE;

    pos3D = Vector3{ pos2D.y, 0, pos2D.x };
}

Node::~Node() {
    // PRINT("Unloading Node");
}

void Node::Draw() {
    DrawModel(model, pos3D, 1, color);
}

Vector2 Node::GetPos() {
    return pos2D;
}

int Node::GetSize() {
    return connectedRoads.size();
}

void Node::AddRoad(RoadSegment* road) {
    connectedRoads.push_back(road);
}

void Node::RemoveRoad(RoadSegment* road) {
    connectedRoads.erase(remove(connectedRoads.begin(), connectedRoads.end(), road), connectedRoads.end());
    connectedRoads.shrink_to_fit();
}

std::vector<RoadSegment*> Node::GetConnectedRoads() {
    return connectedRoads;
}