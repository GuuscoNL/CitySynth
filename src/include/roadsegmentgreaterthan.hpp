#pragma once
#include "roadsegment.hpp"

struct RoadSegmentGreaterThan {
    /**
     * @brief Based on the delay of the RoadSegment
     *
     * @param lhs Left Hand Side RoadSegment
     * @param rhs Right Hand Side RoadSegment
     * @return true lhs > rhs (Based on delay attribute)
     * @return false lhs <= rhs (Based on delay attribute)
     */
    bool operator()(const RoadSegment* lhs, const RoadSegment* rhs) const;
};