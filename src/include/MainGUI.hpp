/*******************************************************************************************
*
*   MainGUI v1.0.0 - Main GUI
*
*   MODULE USAGE:
*       #define GUI_MAINGUI_IMPLEMENTATION
*       #include "gui_MainGUI.h"
*
*       INIT: GuiMainGUIState state = InitGuiMainGUI();
*       DRAW: GuiMainGUI(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 Guus ten Hagen. All Rights Reserved.
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

#ifndef GUI_MAINGUI_H
#define GUI_MAINGUI_H

#define PRINT(x) std::cout << x << std::endl

typedef struct {
    Vector2 anchorMain;
    Vector2 anchorDebug;
    Vector2 anchorHighway;
    Vector2 anchorGeneral;
    Vector2 anchorControls;
    Vector2 anchorSideRoad;
    Vector2 anchorHeatmap;
    Vector2 anchorCustomHeatmap;
    Vector2 anchorInfo;
    
    bool windowMainActive;
    bool windowDebugActive;
    bool inputHighwayLengthEditMode;
    char inputHighwayLengthText[128];
    bool inputHighwayWidthEditMode;
    char inputHighwayWidthText[128];
    bool inputHighwayHeightEditMode;
    char inputHighwayHeightText[128];
    bool inputMaxAngleEditMode;
    char inputMaxAngleText[128];
    bool inputSampleAmountEditMode;
    int inputSampleAmountValue;
    bool inputHighwayBranchChanceEditMode;
    char inputHighwayBranchChanceText[128];
    bool inputHighwaySideRoadChanceEditMode;
    char inputHighwaySideRoadChanceText[128];
    bool inputSeedEditMode;
    int inputSeedValue;
    bool inputSegmentLimitEditMode;
    int inputSegmentLimitValue;
    bool inputSizeEditMode;
    int inputSizeValue;
    bool inputCityNameEditMode;
    char inputCityNameText[128];
    bool inputSideRoadLengthEditMode;
    char inputSideRoadLengthText[128];
    bool inputSideRoadWidthEditMode;
    char inputSideRoadWidthText[128];
    bool inputSideRoadHeightEditMode;
    char inputSideRoadHeightText[128];
    bool inputSideRoadBranchChanceEditMode;
    char inputSideRoadBranchChanceText[128];
    bool inputThresholdEditMode;
    char inputThresholdText[128];
    bool inputFrequencyEditMode;
    char inputFrequencyText[128];
    bool inputAmplitudeEditMode;
    char inputAmplitudeText[128];
    bool inputLacunarityEditMode;
    char inputLacunarityText[128];
    bool inputOctavesEditMode;
    int inputOctavesValue;
    bool inputCustomHeatmapEditMode;
    char inputCustomHeatmapText[128];
    Settings* settings;
    char labelDiscoRoads[13];
    char labelShowNodes[11];
    char labelinfo[33];
    City* city;
    bool DiscoRoads;

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
static void ButtonShowNodes();
static void ButtonDiscoRoads();
static void ButtonResetCity();
static void ButtonExportCity();
static void ButtonGenerateCity();
static void ButtonGenerateHeatmap();
static void ButtonCustomHeatmap();

#ifdef __cplusplus
}
#endif

#endif // GUI_MAINGUI_H

/***********************************************************************************
*
*   GUI_MAINGUI IMPLEMENTATION
*
************************************************************************************/
// #if defined(GUI_MAINGUI_IMPLEMENTATION)

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
GuiMainGUIState InitGuiMainGUI(int screenWidth, Settings* settings, City* city)
{
    GuiMainGUIState state = { 0 };

    state.anchorMain = (Vector2){ screenWidth - 424.f, 0 };
    state.anchorDebug = (Vector2){ 0, 88 };
    state.anchorHighway = (Vector2){ state.anchorMain.x + 8, 32 };
    state.anchorGeneral = (Vector2){ state.anchorMain.x + 8, 224 };
    state.anchorControls = (Vector2){ state.anchorMain.x + 8, 344 };
    state.anchorSideRoad = (Vector2){ state.anchorMain.x + 216, 32 };
    state.anchorHeatmap = (Vector2){ state.anchorMain.x + 216, 176 };
    state.anchorCustomHeatmap = (Vector2){ state.anchorMain.x + 216, 312 };
    state.anchorInfo = (Vector2){ state.anchorMain.x + 216, 392 };
    
    state.windowMainActive = true;
    state.windowDebugActive = true;
    state.inputHighwayLengthEditMode = false;
    strcpy(state.inputHighwayLengthText, "1.4");
    state.inputHighwayWidthEditMode = false;
    strcpy(state.inputHighwayWidthText, "0.25");
    state.inputHighwayHeightEditMode = false;
    strcpy(state.inputHighwayHeightText, "0.041");
    state.inputMaxAngleEditMode = false;
    strcpy(state.inputMaxAngleText, "15");
    state.inputSampleAmountEditMode = false;
    state.inputSampleAmountValue = 10;
    state.inputHighwayBranchChanceEditMode = false;
    strcpy(state.inputHighwayBranchChanceText, "2");
    state.inputHighwaySideRoadChanceEditMode = false;
    strcpy(state.inputHighwaySideRoadChanceText, "80");
    state.inputSeedEditMode = false;
    state.inputSeedValue = 0;
    state.inputSegmentLimitEditMode = false;
    state.inputSegmentLimitValue = 1000;
    state.inputSizeEditMode = false;
    state.inputSizeValue = 300;
    state.inputCityNameEditMode = false;
    strcpy(state.inputCityNameText, "");
    state.inputSideRoadLengthEditMode = false;
    strcpy(state.inputSideRoadLengthText, "0.7");
    state.inputSideRoadWidthEditMode = false;
    strcpy(state.inputSideRoadWidthText, "0.1");
    state.inputSideRoadHeightEditMode = false;
    strcpy(state.inputSideRoadHeightText, "0.04");
    state.inputSideRoadBranchChanceEditMode = false;
    strcpy(state.inputSideRoadBranchChanceText, "80");
    state.inputThresholdEditMode = false;
    strcpy(state.inputThresholdText, "0.5");
    state.inputFrequencyEditMode = false;
    strcpy(state.inputFrequencyText, "0.04");
    state.inputAmplitudeEditMode = false;
    strcpy(state.inputAmplitudeText, "0.5");
    state.inputLacunarityEditMode = false;
    strcpy(state.inputLacunarityText, "25");
    state.inputOctavesEditMode = false;
    state.inputOctavesValue = 1;
    state.inputCustomHeatmapEditMode = false;
    strcpy(state.inputCustomHeatmapText, "");
    state.settings = settings;
    strcpy(state.labelShowNodes, "Show Nodes");
    strcpy(state.labelDiscoRoads, "Disco Roads");
    strcpy(state.labelinfo, "");
    state.city = city;
    state.DiscoRoads = false;

    // Custom variables initialization

    return state;
}
static void ButtonShowNodes(GuiMainGUIState* state)
{
    if (state->settings->ShowNodes) {
        state->settings->ShowNodes = false; 
        strcpy(state->labelShowNodes, "Show Nodes");
    } else {
        state->settings->ShowNodes = true; 
        strcpy(state->labelShowNodes, "Hide Nodes");
    }
}
static void ButtonDiscoRoads(GuiMainGUIState* state)
{
    if (state->DiscoRoads) {
        state->DiscoRoads = false;
        strcpy(state->labelDiscoRoads, "Disco Roads");
        for (RoadSegment* road : state->city->GetRoads()) {
            if (road->GetType() == RoadSegment::SIDEROAD) {
                road->SetColor(GRAY);
            } else if (road->GetType() == RoadSegment::HIGHWAY) {
                road->SetColor(DARKGRAY);
            }
        }
    } else {
        state->DiscoRoads = true;
        strcpy(state->labelDiscoRoads, "Normal Roads");
        for (RoadSegment* road : state->city->GetRoads()) {
            
            road->SetColor(Color{ static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)),
                  static_cast<unsigned char>(GetRandomValue(0,255)) });
            
        }
    }
}
static void ButtonResetCity(GuiMainGUIState* state)
{
    state->city->ResetCity();
}
static void ButtonExportCity()
{
    PRINT("Export City: TODO");
    // TODO: Implement control logic
}
static void ButtonGenerateCity(GuiMainGUIState* state)
{
    Settings& settings = *state->settings;

    if (atof(state->inputHighwayLengthText) > 0 && atof(state->inputHighwayLengthText) <= 100) {
        settings.highwayLength = atof(state->inputHighwayLengthText);
    } else {
        strcpy(state->labelinfo, "HighwayLength: 0-100");
        return;
    }

    if (atof(state->inputHighwayWidthText) > 0 && atof(state->inputHighwayWidthText) <= 50) {
        settings.highwayWidth = atof(state->inputHighwayWidthText);
    } else {
        strcpy(state->labelinfo, "HighwayWidth: 0-50");
        return;
    }

    // Already has min and max value
    settings.highwaySampleAmount = state->inputSampleAmountValue;
  
    if (atof(state->inputMaxAngleText) > 0 && atof(state->inputMaxAngleText) <= 180) {
        settings.highwayAngle = atof(state->inputMaxAngleText);
    } else {
        strcpy(state->labelinfo, "HighwayAngle: 0-180");
        return;
    }

    if (atof(state->inputHighwayHeightText) > 0 && atof(state->inputHighwayHeightText) <= 50) {
        settings.highwayHeight = atof(state->inputHighwayHeightText);
    } else {
        strcpy(state->labelinfo, "HighwayHeight: 0-50");
        return;
    }

    if (atof(state->inputHighwayBranchChanceText) > 0 && atof(state->inputHighwayBranchChanceText) <= 100) {
        settings.highwayBranchChance = atof(state->inputHighwayBranchChanceText);
    } else {
        strcpy(state->labelinfo, "HighwayBranchChance: 0-100");
        return;
    }

    if (atof(state->inputHighwaySideRoadChanceText) > 0 && atof(state->inputHighwaySideRoadChanceText) <= 100) {
        settings.highwaySideRoadBranchChance = atof(state->inputHighwaySideRoadChanceText);
    } else {
        strcpy(state->labelinfo, "HighwaySideRoadBranchChan: 0-100");
        return;
    }

    if (atof(state->inputSideRoadLengthText) > 0 && atof(state->inputSideRoadLengthText) <= 100) {
        settings.sideRoadLength = atof(state->inputSideRoadLengthText);
    } else {
        strcpy(state->labelinfo, "SideRoadLength: 0-100");
        return;
    }

    if (atof(state->inputSideRoadWidthText) > 0 && atof(state->inputSideRoadWidthText) <= 50) {
        settings.sideRoadWidth = atof(state->inputSideRoadWidthText);
    } else {
        strcpy(state->labelinfo, "SideRoadWidth: 0-50");
        return;
    }

    if (atof(state->inputSideRoadHeightText) > 0 && atof(state->inputSideRoadHeightText) <= 50) {
        settings.sideRoadHeight = atof(state->inputSideRoadHeightText);
    } else {
        strcpy(state->labelinfo, "SideRoadHeight: 0-50");
        return;
    }

    if (atof(state->inputThresholdText) > 0 && atof(state->inputThresholdText) <= 1) {
        settings.sideRoadThreshold = atof(state->inputThresholdText);
    } else {
        strcpy(state->labelinfo, "SideRoadThreshold: 0-1");
        return;
    }

    settings.SetSeed(state->inputSeedValue);

    strcpy(state->labelinfo, "");

    UnloadModel(settings.highwayModel);
    settings.highwayModel = LoadModelFromMesh(GenMeshCube(settings.highwayWidth, settings.highwayHeight, settings.highwayLength));
    UnloadModel(settings.sideRoadModel);
    settings.sideRoadModel = LoadModelFromMesh(GenMeshCube(settings.sideRoadWidth, settings.sideRoadHeight, settings.sideRoadLength));
    state->city->GenerateCity(state->inputSegmentLimitValue);

}
static void ButtonGenerateHeatmap(GuiMainGUIState* state)
{
    Settings& settings = *state->settings;

    if (atof(state->inputFrequencyText) > 0 && atof(state->inputFrequencyText) <= 100) {
        settings.frequency = atof(state->inputFrequencyText);
    } else {
        strcpy(state->labelinfo, "Frequency: 0-100");
        return;
    }

    if (atof(state->inputAmplitudeText) > 0 && atof(state->inputAmplitudeText) <= 100) {
        settings.amplitude = atof(state->inputAmplitudeText);
    } else {
        strcpy(state->labelinfo, "Amplitude: 0-100");
        return;
    }

    if (atof(state->inputLacunarityText) > 0 && atof(state->inputLacunarityText) <= 10000) {
        settings.lacunarity = atof(state->inputLacunarityText);
    } else {
        strcpy(state->labelinfo, "Amplitude: 0-10000");
        return;
    }

    settings.persistence = 1 / settings.lacunarity;

    // Already has min and max value
    settings.octaves = state->inputOctavesValue; // More = more blurry
    state->city->GeneratePopulationHeatmap();
    // TODO: Implement control logic
}
static void ButtonCustomHeatmap()
{
    PRINT("Custom heatmap: TODO");
    // TODO: Implement control logic
}


void GuiMainGUI(GuiMainGUIState *state)
{   

    state->windowMainActive = !GuiWindowBox((Rectangle){ state->anchorMain.x + 0, state->anchorMain.y + 0, 424, 440 }, "City Settings");

    state->windowDebugActive = !GuiWindowBox((Rectangle){ state->anchorDebug.x + 0, state->anchorDebug.y + 0, 120, 96 }, "Debug");
    if (GuiButton((Rectangle){ state->anchorDebug.x + 8, state->anchorDebug.y + 32, 104, 24 }, state->labelShowNodes)) ButtonShowNodes(state); 
    if (GuiButton((Rectangle){ state->anchorDebug.x + 8, state->anchorDebug.y + 64, 104, 24 }, state->labelDiscoRoads)) ButtonDiscoRoads(state); 

    GuiGroupBox((Rectangle){ state->anchorHighway.x + 0, state->anchorHighway.y + 0, 200, 184 }, "Highway");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 8, 88, 24 }, state->inputHighwayLengthText, 128, state->inputHighwayLengthEditMode)) state->inputHighwayLengthEditMode = !state->inputHighwayLengthEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 8, 96, 24 }, "Length:");
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 32, 96, 24 }, "Width:");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 32, 88, 24 }, state->inputHighwayWidthText, 128, state->inputHighwayWidthEditMode)) state->inputHighwayWidthEditMode = !state->inputHighwayWidthEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 56, 96, 24 }, "Height:");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 56, 88, 24 }, state->inputHighwayHeightText, 128, state->inputHighwayHeightEditMode)) state->inputHighwayHeightEditMode = !state->inputHighwayHeightEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 80, 96, 24 }, "Max angle:");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 80, 88, 24 }, state->inputMaxAngleText, 128, state->inputMaxAngleEditMode)) state->inputMaxAngleEditMode = !state->inputMaxAngleEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 104, 96, 24 }, "Sample amount:");
    if (GuiValueBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 104, 88, 24 }, NULL, &state->inputSampleAmountValue, 0, 100, state->inputSampleAmountEditMode)) state->inputSampleAmountEditMode = !state->inputSampleAmountEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 128, 96, 24 }, "Branch chance:");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 128, 88, 24 }, state->inputHighwayBranchChanceText, 128, state->inputHighwayBranchChanceEditMode)) state->inputHighwayBranchChanceEditMode = !state->inputHighwayBranchChanceEditMode;
    GuiLabel((Rectangle){ state->anchorHighway.x + 8, state->anchorHighway.y + 152, 96, 24 }, "SideRoad chance:");
    if (GuiTextBox((Rectangle){ state->anchorHighway.x + 104, state->anchorHighway.y + 152, 88, 24 }, state->inputHighwaySideRoadChanceText, 128, state->inputHighwaySideRoadChanceEditMode)) state->inputHighwaySideRoadChanceEditMode = !state->inputHighwaySideRoadChanceEditMode;
    GuiGroupBox((Rectangle){ state->anchorGeneral.x + 0, state->anchorGeneral.y + 0, 200, 112 }, "General");
    GuiLabel((Rectangle){ state->anchorGeneral.x + 8, state->anchorGeneral.y + 8, 96, 24 }, "Seed:");
    if (GuiSpinner((Rectangle){ state->anchorGeneral.x + 96, state->anchorGeneral.y + 8, 96, 24 }, NULL, &state->inputSeedValue, 0, 999999, state->inputSeedEditMode)) state->inputSeedEditMode = !state->inputSeedEditMode;
    GuiLabel((Rectangle){ state->anchorGeneral.x + 8, state->anchorGeneral.y + 32, 96, 24 }, "Segment limit:");
    if (GuiValueBox((Rectangle){ state->anchorGeneral.x + 96, state->anchorGeneral.y + 32, 96, 24 }, NULL, &state->inputSegmentLimitValue, 0, 30000, state->inputSegmentLimitEditMode)) state->inputSegmentLimitEditMode = !state->inputSegmentLimitEditMode;
    GuiLabel((Rectangle){ state->anchorGeneral.x + 8, state->anchorGeneral.y + 56, 96, 24 }, "Size:");
    if (GuiValueBox((Rectangle){ state->anchorGeneral.x + 96, state->anchorGeneral.y + 56, 96, 24 }, NULL, &state->inputSizeValue, 0, 2000, state->inputSizeEditMode)) state->inputSizeEditMode = !state->inputSizeEditMode;
    GuiLabel((Rectangle){ state->anchorGeneral.x + 8, state->anchorGeneral.y + 80, 56, 24 }, "City name:");
    if (GuiTextBox((Rectangle){ state->anchorGeneral.x + 64, state->anchorGeneral.y + 80, 128, 24 }, state->inputCityNameText, 128, state->inputCityNameEditMode)) state->inputCityNameEditMode = !state->inputCityNameEditMode;
    GuiGroupBox((Rectangle){ state->anchorControls.x + 0, state->anchorControls.y + 0, 200, 88 }, "Controls");
    if (GuiButton((Rectangle){ state->anchorControls.x + 8, state->anchorControls.y + 8, 72, 24 }, "Reset City")) ButtonResetCity(state); 
    if (GuiButton((Rectangle){ state->anchorControls.x + 88, state->anchorControls.y + 8, 104, 24 }, "Export City")) ButtonExportCity(); 
    if (GuiButton((Rectangle){ state->anchorControls.x + 8, state->anchorControls.y + 40, 184, 40 }, "Generate City")) ButtonGenerateCity(state); 
    GuiGroupBox((Rectangle){ state->anchorSideRoad.x + 0, state->anchorSideRoad.y + 0, 200, 136 }, "SideRoad");
    GuiLabel((Rectangle){ state->anchorSideRoad.x + 8, state->anchorSideRoad.y + 8, 96, 24 }, "Length:");
    GuiLabel((Rectangle){ state->anchorSideRoad.x + 8, state->anchorSideRoad.y + 32, 96, 24 }, "Width:");
    GuiLabel((Rectangle){ state->anchorSideRoad.x + 8, state->anchorSideRoad.y + 56, 96, 24 }, "Height:");
    if (GuiTextBox((Rectangle){ state->anchorSideRoad.x + 104, state->anchorSideRoad.y + 8, 88, 24 }, state->inputSideRoadLengthText, 128, state->inputSideRoadLengthEditMode)) state->inputSideRoadLengthEditMode = !state->inputSideRoadLengthEditMode;
    if (GuiTextBox((Rectangle){ state->anchorSideRoad.x + 104, state->anchorSideRoad.y + 32, 88, 24 }, state->inputSideRoadWidthText, 128, state->inputSideRoadWidthEditMode)) state->inputSideRoadWidthEditMode = !state->inputSideRoadWidthEditMode;
    if (GuiTextBox((Rectangle){ state->anchorSideRoad.x + 104, state->anchorSideRoad.y + 56, 88, 24 }, state->inputSideRoadHeightText, 128, state->inputSideRoadHeightEditMode)) state->inputSideRoadHeightEditMode = !state->inputSideRoadHeightEditMode;
    GuiLabel((Rectangle){ state->anchorSideRoad.x + 8, state->anchorSideRoad.y + 80, 96, 24 }, "Branch chance:");
    if (GuiTextBox((Rectangle){ state->anchorSideRoad.x + 104, state->anchorSideRoad.y + 80, 88, 24 }, state->inputSideRoadBranchChanceText, 128, state->inputSideRoadBranchChanceEditMode)) state->inputSideRoadBranchChanceEditMode = !state->inputSideRoadBranchChanceEditMode;
    GuiLabel((Rectangle){ state->anchorSideRoad.x + 8, state->anchorSideRoad.y + 104, 96, 24 }, "Pop threshold:");
    if (GuiTextBox((Rectangle){ state->anchorSideRoad.x + 104, state->anchorSideRoad.y + 104, 88, 24 }, state->inputThresholdText, 128, state->inputThresholdEditMode)) state->inputThresholdEditMode = !state->inputThresholdEditMode;
    GuiGroupBox((Rectangle){ state->anchorHeatmap.x + 0, state->anchorHeatmap.y + 0, 200, 208 }, "Heatmap");
    GuiLabel((Rectangle){ state->anchorHeatmap.x + 8, state->anchorHeatmap.y + 8, 96, 24 }, "Frequency:");
    if (GuiTextBox((Rectangle){ state->anchorHeatmap.x + 104, state->anchorHeatmap.y + 8, 88, 24 }, state->inputFrequencyText, 128, state->inputFrequencyEditMode)) state->inputFrequencyEditMode = !state->inputFrequencyEditMode;
    GuiLabel((Rectangle){ state->anchorHeatmap.x + 8, state->anchorHeatmap.y + 32, 96, 24 }, "Amplitude:");
    if (GuiTextBox((Rectangle){ state->anchorHeatmap.x + 104, state->anchorHeatmap.y + 32, 88, 24 }, state->inputAmplitudeText, 128, state->inputAmplitudeEditMode)) state->inputAmplitudeEditMode = !state->inputAmplitudeEditMode;
    GuiLabel((Rectangle){ state->anchorHeatmap.x + 8, state->anchorHeatmap.y + 56, 96, 24 }, "Lacunarity:");
    if (GuiTextBox((Rectangle){ state->anchorHeatmap.x + 104, state->anchorHeatmap.y + 56, 88, 24 }, state->inputLacunarityText, 128, state->inputLacunarityEditMode)) state->inputLacunarityEditMode = !state->inputLacunarityEditMode;
    GuiLabel((Rectangle){ state->anchorHeatmap.x + 8, state->anchorHeatmap.y + 80, 96, 24 }, "Octaves:");
    if (GuiSpinner((Rectangle){ state->anchorHeatmap.x + 104, state->anchorHeatmap.y + 80, 88, 24 }, NULL, &state->inputOctavesValue, 1, 100, state->inputOctavesEditMode)) state->inputOctavesEditMode = !state->inputOctavesEditMode;
    if (GuiButton((Rectangle){ state->anchorHeatmap.x + 104, state->anchorHeatmap.y + 104, 88, 24 }, "Generate")) ButtonGenerateHeatmap(state); 
    GuiGroupBox((Rectangle){ state->anchorCustomHeatmap.x + 0, state->anchorCustomHeatmap.y + 0, 200, 72 }, "Custom Heatmap");
    GuiLabel((Rectangle){ state->anchorCustomHeatmap.x + 8, state->anchorCustomHeatmap.y + 8, 56, 24 }, "File name:");
    if (GuiTextBox((Rectangle){ state->anchorCustomHeatmap.x + 64, state->anchorCustomHeatmap.y + 8, 128, 24 }, state->inputCustomHeatmapText, 128, state->inputCustomHeatmapEditMode)) state->inputCustomHeatmapEditMode = !state->inputCustomHeatmapEditMode;
    if (GuiButton((Rectangle){ state->anchorCustomHeatmap.x + 72, state->anchorCustomHeatmap.y + 40, 120, 24 }, "Import")) ButtonCustomHeatmap(); 
    GuiGroupBox((Rectangle){ state->anchorInfo.x + 0, state->anchorInfo.y + 0, 200, 40 }, "Info");
    GuiLabel((Rectangle){ state->anchorInfo.x + 8, state->anchorInfo.y + 8, 184, 24 }, state->labelinfo);


    static bool WasEditingSize;
    if (WasEditingSize && !state->inputSizeEditMode) {
        state->city->SetSize(state->inputSizeValue);
    }

    WasEditingSize = state->inputSizeEditMode;
}

// #endif // GUI_MAINGUI_IMPLEMENTATION
