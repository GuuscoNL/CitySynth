
#include "include/node.hpp"
#include <algorithm>

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

void Node::Draw() const {
    DrawModel(model, pos3D, 1, color);
}

Vector2 Node::GetPos() const {
    return pos2D;
}

int Node::GetAmountConnectedRoads() const {
    return connectedRoads.size();
}

void Node::AddRoad(RoadSegment* const road) {
    connectedRoads.push_back(road);
}

void Node::RemoveRoad(const RoadSegment* const road) {
    connectedRoads.erase(remove(connectedRoads.begin(), connectedRoads.end(), road), connectedRoads.end());
    connectedRoads.shrink_to_fit();
}

std::vector<RoadSegment*> Node::GetConnectedRoads() const {
    return connectedRoads;
}

int Node::GetId() const {
    return id;
}