#include "include/settings.hpp"

Settings::Settings(Shader lightingShader) {
    highwayLength = 1.4;
    highwayWidth = 0.25;
    highwaySampleAmount = 10;
    highwayAngle = 15;
    highwayHeight = 0.045;
    highwayBranchChance = 2; // in procenten
    highwaySideRoadBranchChance = 80; // in procenten

    sideRoadLength = 0.7;
    sideRoadWidth = 0.1;
    sideRoadHeight = 0.04;
    sideRoadBranchChance = 65;
    sideRoadBranchDelay = 7;
    sideRoadThreshold = 0.5; // 0..1

    CloseCrossing = 0.3;
    CloseRoad = 0.2;

    frequency = 0.042; // bigger = smaller patches
    amplitude = 0.5; // ?
    lacunarity = 30; // How "dense" the patches are?
    persistence = 2 / lacunarity; // ?
    octaves = 4; // Less is more smooth?

    highwayModel = LoadModelFromMesh(GenMeshCube(highwayWidth, highwayHeight, highwayLength));
    sideRoadModel = LoadModelFromMesh(GenMeshCube(sideRoadWidth, sideRoadHeight, sideRoadLength));
    NodeModel = LoadModelFromMesh(GenMeshCylinder(0.1, 0.07, 10));

    shader = lightingShader;

    ShowNodes = false;
    useCustomHeatmap = false;
    customHeatmap = GenImageColor(10, 10, WHITE); // temp Image
}