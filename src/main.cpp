#include <fstream>
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
#include "MomentSolver.h"
#include "Section.h"
#include "Interface.h"
#include "Combination.h"

using std::cout;
using std::endl;
using std::vector;

int main()
{
	std::ofstream logFile("log_resultados.txt");

	if (!logFile.is_open())
	{
		std::cerr << "Erro ao abrir o arquivo de log" << std::endl;
		return 1;
	}

	std::streambuf* coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(logFile.rdbuf());

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

	std::cout.rdbuf(coutbuf);

	logFile.close();

	return 0;
}