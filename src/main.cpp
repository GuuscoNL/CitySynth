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

    // ----- Models -----

    Model roadModel = LoadModelFromMesh(GenMeshCube(0.5, .1, 2.0));

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
    settings.highwayLength = 1.5;
    settings.highwayWidth = 0.2;
    settings.highwaySampleAmount = 5;
    settings.highwayAngle = 20;
    settings.highwayHeight = 0.05;

    settings.frequency = 0.005; // bigger = smaller patches
    settings.amplitude = 0.5; // ?
    settings.lacunarity = 2; // ?
    settings.persistence = 0.5; // ?
    settings.octaves = 4; // More = more blurry

    settings.shader = lightingShader;
    settings.SetSeed(0);

    // ----- City -----
    City city = City(200.f, &settings);
    city.GeneratePopulationHeatmap(20, 5, 0.9);
    city.City::GenerateCity(50);

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

            ClearBackground(RAYWHITE);

            // ----- Draw 3D -----
            {
                BeginMode3D(camera);

                city.Draw();

                EndMode3D();
            }

            // ----- UI -----
            DrawFPS(10, 10);
            DrawText("Hold down right mouse button to move the camera", 10, 60, 20, DARKGRAY);

            EndDrawing();
        }
        totalTime += GetFrameTime();
    }

    UnloadModel(roadModel);
    UnloadShader(lightingShader);
    CloseWindow();

    return 0;
}
