#pragma once

#include "roadsegment.hpp"

class SideRoad : public RoadSegment {
public:
    /**
     * @brief Construct a new SideRoad object.
     *
     * @param delay The delay.
     * @param settings Pointer to Settings.
     * @param from The Node where this SideRoad starts.
     * @param to The Node where this SideRoad ends.
     */
    SideRoad(int delay, Settings* settings, Node* from, Node* to);

private:
};