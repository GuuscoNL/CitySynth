#pragma once
#include "roadsegment.hpp"

class Highway : public RoadSegment {
public:
    /**
     * @brief Construct a new Highway object.
     * 
     * @param delay The delay.
     * @param settings Pointer to Settings.
     * @param from The Node where this Highway starts.
     * @param to The Node where this Highway ends.
     */
    Highway(int delay, Settings* settings, Node* from, Node* to);

private:
};