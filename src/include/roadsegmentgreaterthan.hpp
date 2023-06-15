#pragma once
#include "roadsegment.hpp"

struct RoadSegmentGreaterThan {
    bool operator()(const RoadSegment lhs, const RoadSegment rhs) const;
};