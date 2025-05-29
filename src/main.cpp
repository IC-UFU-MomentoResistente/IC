#include <iostream>

#include "raylib.h"
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
#include "MomentCapacitySolver.h"
#include "Section.h"
#include "Interface.h"
#include "Combination.h"

using std::cout;
using std::endl;
using std::vector;

int main()
{
	Section section;
	Interface interface;

	interface.initInterface();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

		interface.showPrimaryMenuBar(section);
		interface.showSecondaryMenuBar(section);
		interface.crossSectionPlotInterface(section, 56);
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