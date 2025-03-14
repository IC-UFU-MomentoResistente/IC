#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "AppView.h"

using std::cout;
using std::endl;

int main()
{
	Polygon polygon;
	Reinforcement reinforcement;
	AppView view;

	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "IC - FCO - Momentos Resistentes");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;

	ImPlot::CreateContext();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Arquivo"))
			{
				ImGui::MenuItem("Salvar");
				ImGui::MenuItem("Carregar");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Sobre"))
			{
				ImGui::Text("Versao 1.0");
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		bool sectionWindow = true;

		if (sectionWindow)
		{
			ImGui::Begin("Secao Transversal");

			static double coordXPolygon, coordYPolygon;
			ImGui::Text("Vertices");
			ImGui::InputDouble("x (cm)", &coordXPolygon);
			ImGui::InputDouble("y (cm)", &coordYPolygon);

			if (ImGui::Button("Adicionar Ponto"))
			{
				polygon.addVertice(coordXPolygon, coordYPolygon);
			}

			ImGui::SameLine();

			if (ImGui::Button("Remover Ponto"))
			{
				if (!polygon.getPolygonVertices().empty())
					polygon.removeLastVertice();
			}

			ImGui::SameLine();

			if (ImGui::Button("Calcular parametros"))
			{
				polygon.computeArea();
				polygon.computeCentroid();
				polygon.computeMaxCoordY();
				polygon.computeMinCoordY();
				polygon.computeHeight();
			}

			ImGui::SameLine();

			if (ImGui::Button("Transladar"))
			{
				polygon.translateToCentroid();
				reinforcement.translateToCentroidPolygon(polygon.getGeometricCenter());
			}

			ImGui::SameLine();

			if (ImGui::Button("Rotacionar"))
			{
				polygon.rotateAroundCentroid(10);
				reinforcement.rotateAroundCentroidPolygon(10, polygon.getGeometricCenter());
			}

			ImGui::Text("Area: %.2f", polygon.getPolygonArea());
			ImGui::Text("MaxY: %.2f", polygon.getMaxY());
			ImGui::Text("MinY: %.2f", polygon.getMinY());
			ImGui::Text("Height: %.2f", polygon.getMaxY());
			ImGui::Text("CG: %.2f, %.2f", polygon.getGeometricCenter().getX(), polygon.getGeometricCenter().getY());
			ImGui::Text("Vet0: %.2f, %.2f", polygon.getVet0X(), polygon.getVet0Y());

			// finaliza a janela no ImGui
			ImGui::End();
		}

		bool reinforcementWindow = true;

		static int barMode, numBar = 0;
		static double coordXBar, coordYBar, diameterBar, coordXiBar, coordXfBar, coordYiBar, coordYfBar;

		if (reinforcementWindow)
		{
			ImGui::Begin("Entrada de dados: Armadura Passiva");
			ImGui::RadioButton("Uma barra", &barMode, 0);
			ImGui::RadioButton("Linha de barras", &barMode, 1);


			if (barMode == 0)
			{
				ImGui::InputDouble("Diametro da barra (mm)", &diameterBar);
				ImGui::InputDouble("x (cm)", &coordXBar);
				ImGui::InputDouble("y (cm)", &coordYBar);

				if (diameterBar < 0)
					diameterBar = 0;

				if (ImGui::Button("Adicionar"))
					reinforcement.addReinforcement(coordXBar, coordYBar, diameterBar);

				ImGui::SameLine();

				if (ImGui::Button("Remover"))
				{
					if (!reinforcement.getReinforcement().empty())
						reinforcement.removeLastBar();
				}
			}

			if (barMode == 1)
			{
				ImGui::InputInt("Numero de barras na linha", &numBar);
				ImGui::InputDouble("Diametro das barras (mm)", &diameterBar);
				ImGui::InputDouble("xi (cm)", &coordXiBar);
				ImGui::InputDouble("xf (cm)", &coordXfBar);
				ImGui::InputDouble("yi (cm)", &coordYiBar);
				ImGui::InputDouble("yf (cm)", &coordYfBar);

				if (diameterBar < 0)
					diameterBar = 0;

				if (numBar < 2)
					numBar = 2;

				double stepX = (coordXfBar - coordXiBar) / (numBar - 1);
				double stepY = (coordYfBar - coordYiBar) / (numBar - 1);

				if (ImGui::Button("Adicionar"))
				{
					for (int i = 0; i < numBar; i++)
					{
						double coordX = coordXiBar + stepX * i;
						double coordY = coordYiBar + stepY * i;

						reinforcement.addReinforcement(coordX, coordY, diameterBar);
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Remover"))
				{
					if (!reinforcement.getReinforcement().empty())
						reinforcement.removeLastBar();
				}
			}

			ImGui::End();
		}

		bool crossSectionPlotWindow = true;

		if (crossSectionPlotWindow)
		{
			ImGui::Begin("Grafico da Secao Transversal");

			ImVec2 plotSize = ImGui::GetContentRegionAvail();

			// inicialização do gráfico com os eixos
			if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
			{
				if (polygon.getPolygonVertices().size() > 2)
				{
					view.renderPolygon(polygon.getPolygonVertices(), "Vertices", "Polygon");
					view.renderReinforcement(reinforcement.getReinforcement(), "Barras");
				}
			}

			ImPlot::EndPlot();

			ImGui::End();
		}

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