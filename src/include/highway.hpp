#pragma once
#include "roadsegment.hpp"

class Highway : public RoadSegment {
public:
    Highway(int delay, Settings* settings, Node* from, Node* to);

private:
};