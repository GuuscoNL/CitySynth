#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <array>
#include <iostream>
#include "include/city.hpp"
#include "include/roadsegment.hpp"

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
    
    Model cube = LoadModelFromMesh(GenMeshCube(0.5, .1, 2.0));

    // ----- lighting shaders -----
    Shader shader = LoadShader("shaders/lighting.vs",
                               "shaders/lighting.fs");

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    std::array<float, 4> ambientValues = { 0.7f, 0.7f, 0.7f, 1.0f };
    SetShaderValue(shader, ambientLoc, ambientValues.data(), SHADER_UNIFORM_VEC4);
    City city = City(100.f);

    city.GetPlane().materials[0].shader = shader;
    cube.materials[0].shader = shader;

    std::array<Light, MAX_LIGHTS> lights{};
    Color sunColour = { 247, 225, 150, 255 };
    Color sunColour2 = {static_cast<unsigned char>(247 * 0.1), 
                        static_cast<unsigned char>(225 * 0.1), 
                        static_cast<unsigned char>(150 * 0.1), 
                        255 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, { -2, 1, -2 }, Vector3Zero(), sunColour, shader);
    lights[1] = CreateLight(LIGHT_DIRECTIONAL, { 2, 1, 2 }, Vector3Zero(), sunColour2, shader);
    

    // ----- Main draw loop -----
    float totalTime = 0;
    int randomSeed = 0;
    SetRandomSeed(randomSeed);

    city.GeneratePopulationHeatmap(0, 0, 1.5);
    RoadSegment road = RoadSegment(cube, Vector3{0,0,0}, 93);
    RoadSegment road1 = RoadSegment(cube, Vector3{0,0,3}, 65);
    RoadSegment road2 = RoadSegment(cube, Vector3{2,0,5}, 41);

    while (!WindowShouldClose()) {
    
        handleCameraControls(camera);

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);
    
        // ----- Draw 2D -----
        BeginDrawing();

            ClearBackground(RAYWHITE);

                // ----- Draw 3D -----
                BeginMode3D(camera);

                road.Draw();
                road1.Draw();
                road2.Draw();

                // for (float i = 0; i < 5; i++){
                //     DrawModel(cube, Vector3{i*2.0f*(totalTime/ 2.0f), 0.05, 0}, 1.0f, WHITE);
                // }
                city.Draw();

                EndMode3D();

            // ----- UI -----
            DrawFPS(10, 10);
            DrawText("Hold down right mouse button to move the camera", 10, 60, 20, DARKGRAY);

        EndDrawing();
        totalTime += GetFrameTime();
    }

    UnloadModel(cube);
    UnloadShader(shader);
    CloseWindow();

    return 0;
}
