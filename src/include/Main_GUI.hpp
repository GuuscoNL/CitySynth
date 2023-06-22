/*******************************************************************************************
*
*   MainGUI v1.0.0 - The main GUI
*
*   MODULE USAGE:
*       #define GUI_MAIN_GUI_IMPLEMENTATION
*       #include "gui_Main_GUI.h"
*
*       INIT: GuiMainGUIState state = InitGuiMainGUI();
*       DRAW: GuiMainGUI(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 guuscoNL. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_MAIN_GUI_H
#define GUI_MAIN_GUI_H

typedef struct {
    Vector2 mainGUIWindowAnchor;
    Vector2 groupboxSideRoadAnchor;
    Vector2 groupboxHeatmapAnchor;
    Vector2 groupboxHighwayAnchor;
    Vector2 debugWindowAnchor;
    
    bool mainGUIWindowActive;
    bool highwayLengthInputEditMode;
    int highwayLengthInputValue;
    bool highwayWidthInputEditMode;
    int highwayWidthInputValue;
    bool sampleCountEditMode;
    int sampleCountValue;
    bool maxAngleInputEditMode;
    int maxAngleInputValue;
    bool highwayHeightInputEditMode;
    int highwayHeightInputValue;
    bool highwayBranchChanceEditMode;
    int highwayBranchChanceValue;
    bool highwaySideroadBranchChanceEditMode;
    int highwaySideroadBranchChanceValue;
    bool sideRoadLengthEditMode;
    int sideRoadLengthValue;
    bool sideRoadwidthEditMode;
    int sideRoadwidthValue;
    bool sideRoadHeightEditMode;
    int sideRoadHeightValue;
    bool sideRoadBranchChanceEditMode;
    int sideRoadBranchChanceValue;
    bool sideRoadThresholdEditMode;
    int sideRoadThresholdValue;
    bool frequencyEditMode;
    int frequencyValue;
    bool amplitudeEditMode;
    int amplitudeValue;
    bool lucanarityEditMode;
    int lucanarityValue;
    bool octavesEditMode;
    int octavesValue;
    bool debugWindowActive;
    bool showNodesActive;
    bool discoRoadsActive;
    bool customHeatmapPathEditMode;
    char customHeatmapPathText[128];
    bool randomSeedEditMode;
    int randomSeedValue;
    bool segmentAmountEditMode;
    int segmentAmountValue;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiMainGUIState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiMainGUIState InitGuiMainGUI(void);
void GuiMainGUI(GuiMainGUIState *state);
static void GenerateHeatmap();
static void ResetCity();
static void GenerateCity();

#ifdef __cplusplus
}
#endif

#endif // GUI_MAIN_GUI_H

/***********************************************************************************
*
*   GUI_MAIN_GUI IMPLEMENTATION
*
************************************************************************************/
// #if defined(GUI_MAIN_GUI_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiMainGUIState InitGuiMainGUI(void)
{
    GuiMainGUIState state = { 0 };

    state.mainGUIWindowAnchor = (Vector2){ SCREEN_WIDTH-176, 0 };
    state.groupboxSideRoadAnchor = (Vector2){ state.mainGUIWindowAnchor.x + 6, state.mainGUIWindowAnchor.y+224 };
    state.groupboxHeatmapAnchor = (Vector2){ state.mainGUIWindowAnchor.x + 6, state.mainGUIWindowAnchor.y+368 };
    state.groupboxHighwayAnchor = (Vector2){ state.mainGUIWindowAnchor.x + 6, state.mainGUIWindowAnchor.y+32 };
    state.debugWindowAnchor = (Vector2){ 0, 88 };
    
    state.mainGUIWindowActive = true;
    state.highwayLengthInputEditMode = false;
    state.highwayLengthInputValue = 0;
    state.highwayWidthInputEditMode = false;
    state.highwayWidthInputValue = 0;
    state.sampleCountEditMode = false;
    state.sampleCountValue = 0;
    state.maxAngleInputEditMode = false;
    state.maxAngleInputValue = 0;
    state.highwayHeightInputEditMode = false;
    state.highwayHeightInputValue = 0;
    state.highwayBranchChanceEditMode = false;
    state.highwayBranchChanceValue = 0;
    state.highwaySideroadBranchChanceEditMode = false;
    state.highwaySideroadBranchChanceValue = 0;
    state.sideRoadLengthEditMode = false;
    state.sideRoadLengthValue = 0;
    state.sideRoadwidthEditMode = false;
    state.sideRoadwidthValue = 0;
    state.sideRoadHeightEditMode = false;
    state.sideRoadHeightValue = 0;
    state.sideRoadBranchChanceEditMode = false;
    state.sideRoadBranchChanceValue = 0;
    state.sideRoadThresholdEditMode = false;
    state.sideRoadThresholdValue = 0;
    state.frequencyEditMode = false;
    state.frequencyValue = 0;
    state.amplitudeEditMode = false;
    state.amplitudeValue = 0;
    state.lucanarityEditMode = false;
    state.lucanarityValue = 0;
    state.octavesEditMode = false;
    state.octavesValue = 0;
    state.debugWindowActive = true;
    state.showNodesActive = true;
    state.discoRoadsActive = true;
    state.customHeatmapPathEditMode = false;
    strcpy(state.customHeatmapPathText, "Custom heatmap path");
    state.randomSeedEditMode = false;
    state.randomSeedValue = 0;
    state.segmentAmountEditMode = false;
    state.segmentAmountValue = 0;

    // Custom variables initialization

    return state;
}
static void GenerateHeatmap()
{
    // TODO: Implement control logic
}
static void ResetCity()
{
    // TODO: Implement control logic
}
static void GenerateCity()
{
    // TODO: Implement control logic
}


void GuiMainGUI(GuiMainGUIState *state)
{
    if (state->mainGUIWindowActive)
    {
        state->mainGUIWindowActive = !GuiWindowBox((Rectangle){ state->mainGUIWindowAnchor.x + 0, state->mainGUIWindowAnchor.y + 0, 176, 640 }, "City settings");
        if (GuiButton((Rectangle){ state->mainGUIWindowAnchor.x + 8, state->mainGUIWindowAnchor.y + 608, 56, 24 }, "RESET")) ResetCity(); 
        if (GuiValueBox((Rectangle){ state->mainGUIWindowAnchor.x + 40, state->mainGUIWindowAnchor.y + 544, 128, 24 }, "Seed", &state->randomSeedValue, 0, 100, state->randomSeedEditMode)) state->randomSeedEditMode = !state->randomSeedEditMode;
        if (GuiButton((Rectangle){ state->mainGUIWindowAnchor.x + 72, state->mainGUIWindowAnchor.y + 608, 96, 24 }, "GENERATE CITY")) GenerateCity(); 
        if (GuiValueBox((Rectangle){ state->mainGUIWindowAnchor.x + 88, state->mainGUIWindowAnchor.y + 576, 80, 24 }, "Segment amount", &state->segmentAmountValue, 0, 100, state->segmentAmountEditMode)) state->segmentAmountEditMode = !state->segmentAmountEditMode;
    }
    if (state->debugWindowActive)
    {
        state->debugWindowActive = !GuiWindowBox((Rectangle){ state->debugWindowAnchor.x + 0, state->debugWindowAnchor.y + 0, 104, 96 }, "Debug");
        state->showNodesActive = GuiToggle((Rectangle){ state->debugWindowAnchor.x + 8, state->debugWindowAnchor.y + 32, 88, 24 }, "Show Nodes", &state->showNodesActive);
        state->discoRoadsActive = GuiToggle((Rectangle){ state->debugWindowAnchor.x + 8, state->debugWindowAnchor.y + 64, 88, 24 }, "Disco roads", &state->discoRoadsActive);
    }
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 8, 56, 24 }, "Length", &state->highwayLengthInputValue, 0, 100, state->highwayLengthInputEditMode)) state->highwayLengthInputEditMode = !state->highwayLengthInputEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 32, 56, 24 }, "Width", &state->highwayWidthInputValue, 0, 100, state->highwayWidthInputEditMode)) state->highwayWidthInputEditMode = !state->highwayWidthInputEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 104, 56, 24 }, "Sample amount", &state->sampleCountValue, 0, 100, state->sampleCountEditMode)) state->sampleCountEditMode = !state->sampleCountEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 80, 56, 24 }, "Max angle", &state->maxAngleInputValue, 0, 100, state->maxAngleInputEditMode)) state->maxAngleInputEditMode = !state->maxAngleInputEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 56, 56, 24 }, "Height", &state->highwayHeightInputValue, 0, 100, state->highwayHeightInputEditMode)) state->highwayHeightInputEditMode = !state->highwayHeightInputEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 128, 56, 24 }, "Branch chance", &state->highwayBranchChanceValue, 0, 100, state->highwayBranchChanceEditMode)) state->highwayBranchChanceEditMode = !state->highwayBranchChanceEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHighwayAnchor.x + 96, state->groupboxHighwayAnchor.y + 152, 56, 24 }, "Sideroad chance", &state->highwaySideroadBranchChanceValue, 0, 100, state->highwaySideroadBranchChanceEditMode)) state->highwaySideroadBranchChanceEditMode = !state->highwaySideroadBranchChanceEditMode;
    GuiGroupBox((Rectangle){ state->groupboxSideRoadAnchor.x + 0, state->groupboxSideRoadAnchor.y + 0, 160, 136 }, "Sideroad");
    if (GuiValueBox((Rectangle){ state->groupboxSideRoadAnchor.x + 96, state->groupboxSideRoadAnchor.y + 8, 56, 24 }, "Length", &state->sideRoadLengthValue, 0, 100, state->sideRoadLengthEditMode)) state->sideRoadLengthEditMode = !state->sideRoadLengthEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxSideRoadAnchor.x + 96, state->groupboxSideRoadAnchor.y + 32, 56, 24 }, "Width", &state->sideRoadwidthValue, 0, 100, state->sideRoadwidthEditMode)) state->sideRoadwidthEditMode = !state->sideRoadwidthEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxSideRoadAnchor.x + 96, state->groupboxSideRoadAnchor.y + 56, 56, 24 }, "Height", &state->sideRoadHeightValue, 0, 100, state->sideRoadHeightEditMode)) state->sideRoadHeightEditMode = !state->sideRoadHeightEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxSideRoadAnchor.x + 96, state->groupboxSideRoadAnchor.y + 80, 56, 24 }, "Branch chance", &state->sideRoadBranchChanceValue, 0, 100, state->sideRoadBranchChanceEditMode)) state->sideRoadBranchChanceEditMode = !state->sideRoadBranchChanceEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxSideRoadAnchor.x + 96, state->groupboxSideRoadAnchor.y + 104, 56, 24 }, "Pop threshold", &state->sideRoadThresholdValue, 0, 100, state->sideRoadThresholdEditMode)) state->sideRoadThresholdEditMode = !state->sideRoadThresholdEditMode;
    GuiGroupBox((Rectangle){ state->groupboxHeatmapAnchor.x + 0, state->groupboxHeatmapAnchor.y + 0, 160, 168 }, "Heatmap");
    if (GuiValueBox((Rectangle){ state->groupboxHeatmapAnchor.x + 96, state->groupboxHeatmapAnchor.y + 8, 56, 24 }, "Frequency", &state->frequencyValue, 0, 100, state->frequencyEditMode)) state->frequencyEditMode = !state->frequencyEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHeatmapAnchor.x + 96, state->groupboxHeatmapAnchor.y + 32, 56, 24 }, "Amplitude", &state->amplitudeValue, 0, 100, state->amplitudeEditMode)) state->amplitudeEditMode = !state->amplitudeEditMode;
    if (GuiValueBox((Rectangle){ state->groupboxHeatmapAnchor.x + 96, state->groupboxHeatmapAnchor.y + 56, 56, 24 }, "Lacunarity", &state->lucanarityValue, 0, 100, state->lucanarityEditMode)) state->lucanarityEditMode = !state->lucanarityEditMode;
    if (GuiSpinner((Rectangle){ state->groupboxHeatmapAnchor.x + 72, state->groupboxHeatmapAnchor.y + 80, 80, 24 }, "Octaves", &state->octavesValue, 0, 100, state->octavesEditMode)) state->octavesEditMode = !state->octavesEditMode;
    GuiGroupBox((Rectangle){ state->groupboxHighwayAnchor.x + 0, state->groupboxHighwayAnchor.y + 0, 160, 184 }, "Highway");
    if (GuiTextBox((Rectangle){ state->groupboxHeatmapAnchor.x + 8, state->groupboxHeatmapAnchor.y + 136, 144, 24 }, state->customHeatmapPathText, 128, state->customHeatmapPathEditMode)) state->customHeatmapPathEditMode = !state->customHeatmapPathEditMode;
    if (GuiButton((Rectangle){ state->groupboxHeatmapAnchor.x + 72, state->groupboxHeatmapAnchor.y + 104, 80, 24 }, "Generate")) GenerateHeatmap(); 
}

// #endif // GUI_MAIN_GUI_IMPLEMENTATION
