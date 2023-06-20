#include "include/highway.hpp"

Highway::Highway(int delay, Settings* settings, Node* from, Node* to) : RoadSegment(delay, settings, from, to){

    model = settings->highwayModel;
    model.materials[0].shader = settings->shader;

    height = settings->highwayHeight;

    modelLength = settings->highwayLength;

    CalculatePosAndAngle();
    color = Color{ static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)) };
    from->AddRoad(this);
    to->AddRoad(this);
}