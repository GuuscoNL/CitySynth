#include "include/highway.hpp"

Highway::Highway(int delay, Settings* settings, Node* from, Node* to) : RoadSegment(delay, settings, from, to){

    model = settings->highwayModel;
    model.materials[0].shader = settings->shader;

    height = settings->highwayHeight;

    modelLength = settings->highwayLength;

    type = HIGHWAY;

    CalculatePosAndAngle();
    color = DARKGRAY;
    from->AddRoad(this);
    to->AddRoad(this);
}