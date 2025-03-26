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

		interface.showPrimaryMenuBar();
		interface.showSecondaryMenuBar(section);
		interface.crossSectionPlotInterface(section, 50);
		interface.RightTablePos("Tabela de Pontos", "Tabela de Esforços", 50, section);
		

		rlImGuiEnd();
		EndDrawing();
	}

	ImPlot::DestroyContext();
	rlImGuiShutdown();
	CloseWindow();

	return 0;
}