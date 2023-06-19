
#include <iostream>
#include "include/node.hpp"

#define PRINT(x) std::cout << x << std::endl

Node::Node(Vector2 pos2D, Settings* settings) :
    pos2D(pos2D),
    settings(settings) {
    model = settings->NodeModel;
    model.materials[0].shader = settings->shader;

    pos3D = Vector3{pos2D.y, 0, pos2D.x};
}

Node::~Node() {
    // PRINT("Unloading Node");
}

void Node::Draw() {
    DrawModel(model, pos3D, 1, GRAY);
}

Vector2 Node::GetPos() {
    return pos2D;
}