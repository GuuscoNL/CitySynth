#include "include/sideroad.hpp"

SideRoad::SideRoad(int delay, Settings* settings, Node* from, Node* to) : RoadSegment(delay, settings, from, to) {

    model = settings->sideRoadModel;
    model.materials[0].shader = settings->shader;

    height = settings->sideRoadHeight;

    modelLength = settings->sideRoadLength;

    type = SIDEROAD;

    CalculatePosAndAngle();
    color = GRAY;
    from->AddRoad(this);
    to->AddRoad(this);
}