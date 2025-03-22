#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "Section.h"
#include "AppView.h"
#include "Interface.h"

using std::cout;
using std::endl;

int main()
{
	Section section;
	AppView view;
	Interface interface;

	interface.initInterface();

	while (!WindowShouldClose()) 
    {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

		interface.showPrimaryMenuBar();
		interface.showSecondaryMenuBar(section, view);
		interface.crossSectionPlotInterface(section, view);

		bool showDemoWindow = true;

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		rlImGuiEnd();
		EndDrawing();
	}

	ImPlot::DestroyContext();
	rlImGuiShutdown();
	CloseWindow();

	return 0;
}