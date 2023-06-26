
#include <iostream>
#include "include/node.hpp"
#include <algorithm>

#define PRINT(x) std::cout << x << std::endl

Node::Node(const Vector2& pos2D, Settings* settings) :
    pos2D(pos2D),
    settings(settings) {
    // Set model
    model = settings->NodeModel;
    model.materials[0].shader = settings->shader;

    color = WHITE;
    id = nextId++;

    pos3D = Vector3{ pos2D.y, 0, pos2D.x };
}

int Node::nextId = 0; // Set nextId to 0 at start of program

void Node::Draw() {
    DrawModel(model, pos3D, 1, color);
}

Vector2 Node::GetPos() const {
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