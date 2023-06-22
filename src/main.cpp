#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <array>
#include <iostream>
#include "include/city.hpp"
#include "include/roadsegment.hpp"
#include "include/settings.hpp"

#define RLIGHTS_IMPLEMENTATION
#include "include/rlights.h"

#define CAMERA_IMPLEMENTATION

#define ASPECT_RATIO 16/9
#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH SCREEN_HEIGHT * ASPECT_RATIO

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#define GUI_MAINGUI_IMPLEMENTATION
#include "include/MainGUI.hpp"


#define PRINT(x) std::cout << x << std::endl
#define PRINTVEC3(vec) std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl

void handleCameraControls(Camera3D& camera) {

    if (!IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) return; // Update camera only when right mouse button is held down

    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [shaders] example - basic lighting");

    // ----- Camera -----
    Camera3D camera{};
    camera.position = { 2.0f, 6.0f, 9.0f };
    camera.target = { 0.0f, 0.5f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    


    // ----- lighting shaders -----
    Shader lightingShader = LoadShader("shaders/lighting.vs",
        "shaders/lighting.fs");

    lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");

    int ambientLoc = GetShaderLocation(lightingShader, "ambient");
    std::array<float, 4> ambientValues = { 0.7f, 0.7f, 0.7f, 1.0f };
    SetShaderValue(lightingShader, ambientLoc, ambientValues.data(), SHADER_UNIFORM_VEC4);

    std::array<Light, MAX_LIGHTS> lights{};
    Color sunColour = { 247, 225, 150, 255 };
    Color sunColour2 = { static_cast<unsigned char>(247 * 0.1),
                        static_cast<unsigned char>(225 * 0.1),
                        static_cast<unsigned char>(150 * 0.1),
                        255 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, { -2, 1, -2 }, Vector3Zero(), sunColour, lightingShader);
    lights[1] = CreateLight(LIGHT_DIRECTIONAL, { 2, 1, 2 }, Vector3Zero(), sunColour2, lightingShader);


    float totalTime = 0;

    // ----- Settings -----
    Settings settings;
    settings.highwayLength = 1.4;
    settings.highwayWidth = 0.25;
    settings.highwaySampleAmount = 10;
    settings.highwayAngle = 15;
    settings.highwayHeight = 0.041;
    settings.highwayBranchChance = 2; // in procenten
    settings.highwaySideRoadBranchChance = 80; // in procenten

    settings.sideRoadLength = 0.7;
    settings.sideRoadWidth = 0.1;
    settings.sideRoadHeight = 0.04;
    settings.sideRoadBranchChance = 80;
    settings.sideRoadBranchDelay = 7;
    settings.sideRoadThreshold = 0.5; // 0..1

    settings.CloseCrossing = 0.3;
    settings.CloseRoad = 0.2;

    settings.frequency = 0.04; // bigger = smaller patches
    settings.amplitude = 0.5; // ?
    settings.lacunarity = 25; // How "dense" the patches are?
    settings.persistence = 1 / settings.lacunarity; // ?
    settings.octaves = 4; // More = more blurry

    settings.highwayModel = LoadModelFromMesh(GenMeshCube(settings.highwayWidth, settings.highwayHeight, settings.highwayLength));
    settings.sideRoadModel = LoadModelFromMesh(GenMeshCube(settings.sideRoadWidth, settings.sideRoadHeight, settings.sideRoadLength));
    settings.NodeModel = LoadModelFromMesh(GenMeshCylinder(0.1, 0.05, 10));

    settings.shader = lightingShader;
    settings.SetSeed(25);

    // ----- City -----
    City city = City(300.f, &settings);
    city.GeneratePopulationHeatmap(20, 5, 0.9);
    city.City::GenerateCity(100);

    // ----- GUI -----
    GuiMainGUIState GUIState = InitGuiMainGUI(SCREEN_WIDTH);

    // ----- Main draw loop -----
    while (!WindowShouldClose()) {

        handleCameraControls(camera);

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW],
            cameraPos, SHADER_UNIFORM_VEC3);

        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(lightingShader, lights[i]);

        // ----- Draw 2D -----
        {
            BeginDrawing();

            ClearBackground(WHITE);

            // ----- Draw 3D -----
            {
                BeginMode3D(camera);

                city.Draw();

                EndMode3D();
            }

            // ----- UI -----
            DrawFPS(10, 10);
            DrawText("Hold down right mouse button to move the camera", 10, 60, 20, DARKGRAY);
            GuiMainGUI(&GUIState);

            EndDrawing();
        }
        totalTime += GetFrameTime();
    }

    UnloadModel(settings.highwayModel);
    UnloadModel(settings.sideRoadModel);
    UnloadModel(settings.NodeModel);
    UnloadShader(lightingShader);
    CloseWindow();

    return 0;
}
