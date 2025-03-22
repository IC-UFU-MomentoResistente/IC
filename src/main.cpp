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

using std::cout;
using std::endl;

int main()
{
	Section section;
	Interface interface;

	SteelProperties steel;

	steel.setParameters(StressStrainSteelModelType::PASSIVE_REINFORCEMENT, 500, 1.15, 210);

	double fyk = steel.getFyk();
	double gammaS = steel.getGammaS();
	double E = steel.getE();
	double fcd = steel.getFyd();
	double strainSteelYield = steel.getStrainSteelYield();
	double strainSteelRupture = steel.getStrainSteelRupture();

	double strain1 = steel.computeStress(-steel.getStrainSteelRupture());
	double strain2 = steel.computeStress(-steel.getStrainSteelYield());
	double strain3 = steel.computeStress(-1);
	double strain4 = steel.computeStress(1);
	double strain5 = steel.computeStress(steel.getStrainSteelYield());
	double strain6 = steel.computeStress(steel.getStrainSteelRupture());

	cout << "fyk: " << fyk << endl;
	cout << "gammaS: " << gammaS << endl;
	cout << "E: " << E << endl;
	cout << "fcd: " << fcd << endl;
	cout << "strainSteelYield: " << strainSteelYield << endl;
	cout << "strainSteelRupture: " << strainSteelRupture << endl;
	cout << "strain1: " << strain1 << endl;
	cout << "strain2: " << strain2 << endl;
	cout << "strain3: " << strain3 << endl;
	cout << "strain4: " << strain4 << endl;
	cout << "strain5: " << strain5 << endl;
	cout << "strain6: " << strain6 << endl;

	interface.initInterface();

	while (!WindowShouldClose()) 
    {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

		interface.showPrimaryMenuBar();
		interface.showSecondaryMenuBar(section);
		interface.crossSectionPlotInterface(section);

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