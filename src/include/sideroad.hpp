#pragma once

#include "roadsegment.hpp"

class SideRoad : public RoadSegment {
public:
    SideRoad(int delay, Settings* settings, Node* from, Node* to);

private:
};