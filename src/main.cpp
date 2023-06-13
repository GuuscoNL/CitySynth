#include "raylib.h"
#include "raymath.h"
#include <array>
#include <iostream>

#define RLIGHTS_IMPLEMENTATION
#include "include/rlights.h"


#define ASPECT_RATIO 16/9
#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH SCREEN_HEIGHT * ASPECT_RATIO

void handleCameraControls(Camera3D& camera) {
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        UpdateCamera(&camera, CAMERA_THIRD_PERSON); // Update camera only when right mouse button is held down
    }
}

int main() {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [shaders] example - basic lighting");

    Camera3D camera{};
    camera.position = { 2.0f, 6.0f, 9.0f };
    camera.target = { 0.0f, 0.5f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
    Model cube = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));

    Shader shader = LoadShader("shaders/lighting.vs",
                               "shaders/lighting.fs");

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    std::array<float, 4> ambientValues = { 0.7f, 0.7f, 0.7f, 1.0f };
    SetShaderValue(shader, ambientLoc, ambientValues.data(), SHADER_UNIFORM_VEC4);

    model.materials[0].shader = shader;
    cube.materials[0].shader = shader;

    std::array<Light, MAX_LIGHTS> lights{};
    Color sunColour = { 247, 225, 100, 255 };
    Color sunColour2 = {static_cast<unsigned char>(247 * 0.1), 
                        static_cast<unsigned char>(225 * 0.1), 
                        static_cast<unsigned char>(100 * 0.1), 
                        255 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, { -2, 1, -2 }, Vector3Zero(), sunColour, shader);
    lights[1] = CreateLight(LIGHT_DIRECTIONAL, { 2, 1, 2 }, Vector3Zero(), sunColour2, shader);
    // lights[2] = CreateLight(LIGHT_POINT, { -2, 1, 2 }, Vector3Zero(), GREEN, shader);
    // lights[3] = CreateLight(LIGHT_POINT, { 2, 1, -2 }, Vector3Zero(), BLUE, shader);

    while (!WindowShouldClose()) {
        handleCameraControls(camera);

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, Vector3Zero(), 1.0f, WHITE);
        DrawModel(cube, Vector3{ 0, 1, 0 }, 1.0f, WHITE);

        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);
        DrawText("Hold down right mouse button to move the camera", 10, 60, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadModel(model);
    UnloadModel(cube);
    UnloadShader(shader);
    CloseWindow();

    return 0;
}
