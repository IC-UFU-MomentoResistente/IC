#include <fstream>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "SteelProperties.h"
#include "StrainDistribution.h"
#include "PolygonStressRegions.h"
#include "AnalyticalIntegration.h"
#include "InternalForces.h"
#include "MomentSolver.h"
#include "Section.h"
#include "Interface.h"
#include "combination.h"
#include "segoeuisl_data.h"

using std::cout;
using std::endl;
using std::vector;

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

int main()
{
	Section section;
	Interface interface;

	int screenWidth = 1280;
    int screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO");
    SetWindowMinSize(320,240);

    // if (!IsWindowReady()) // Verifique se a janela foi criada com sucesso
    // {
    //     std::cerr << "Erro ao criar a janela!" << std::endl;
    //     return; // Saia da função se a janela não foi criada
    // }

    SetTargetFPS(60);

    rlImGuiBeginInitImGui();

    ImGui::StyleColorsDark();

    ImFontConfig fontConfig;
    static const ImWchar customRange[] = {
        0x0020, 0x00FF, // ASCII estendido (contém ², ³)
        0x0370, 0x03FF, // Grego
        0x2070, 0x209F, // Sobrescritos e Subscritos (contém ⁰¹²³⁴⁵⁶⁷⁸⁹)
        0x2030, 0x2030, // Símbolo de por mil (‰)
        0};

    ImGuiIO &io = ImGui::GetIO();
    // ImFont *customFont = io.Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);

    ImFont *customFont = io.Fonts->AddFontFromMemoryTTF(
        (void *)segoeuisl_ttf,
        segoeuisl_ttf_len,
        18.0f,
        &fontConfig,
        customRange);

    if (customFont)
    {
        io.FontDefault = customFont;
    }
    else
    {
        std::cerr << "Erro ao carregar a fonte personalizada!" << std::endl;
    }

    rlImGuiEndInitImGui();
    ImPlot::CreateContext();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

        float scale = MIN((float)GetScreenWidth()/screenWidth, (float)GetScreenHeight()/screenHeight);

        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (screenWidth*scale))*0.5f)/scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (screenHeight*scale))*0.5f)/scale;
        virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)screenWidth, (float)screenHeight });

        // Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
        //SetMouseOffset(-(GetScreenWidth() - (gameScreenWidth*scale))*0.5f, -(GetScreenHeight() - (gameScreenHeight*scale))*0.5f);
        //SetMouseScale(1/scale, 1/scale);

		interface.showPrimaryMenuBar(section);
		interface.showSecondaryMenuBar(section);
		interface.crossSectionPlotInterface(section, 56);
		interface.envelopeMomentsPlotInterface(section, 56);
		interface.RightTablePos("Tabela de Pontos", "Tabela de Esforços", 56, section);

		// bool showDemoWindow = true;
		// if (showDemoWindow)
		// 	ImGui::ShowDemoWindow(&showDemoWindow);

		rlImGuiEnd();
		EndDrawing();
	}

	ImPlot::DestroyContext();
	rlImGuiShutdown();
	CloseWindow();

	return 0;
}