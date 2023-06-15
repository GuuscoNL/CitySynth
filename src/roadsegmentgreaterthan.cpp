#include "include/roadsegmentgreaterthan.hpp"

bool RoadSegmentGreaterThan::operator()(const RoadSegment* lhs, const RoadSegment* rhs) const {
    return lhs->GetDelay() > rhs->GetDelay();
}