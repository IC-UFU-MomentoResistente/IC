#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "AppController.h"
#include "AppView.h"

using std::cout;

int main()
{
    AppController app; 
    AppView view;

    int screenWidth = 1280;
    int screenHeight = 800;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "IC - FCO - Momentos Resistentes");
    SetTargetFPS(60);
    rlImGuiSetup(true);
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

        ImGui::Begin("Secao Transversal");

        static double x, y;
        ImGui::InputDouble("x (cm)", &x);
        ImGui::InputDouble("y (cm)", &y);

        if (ImGui::Button("Adicionar Ponto"))
        {
            app.addVertice(x, y);
        }

        ImVec2 plotSize = ImGui::GetContentRegionAvail();

        // inicialização do gráfico com os eixos
        if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
        {
            if (app.getPolygonVertices().size() > 2)
                view.renderPolygon(app.getPolygonVertices(), "Vertices", "Polygon");
        }

        ImPlot::EndPlot();

        // finaliza a janela no ImGui
        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    ImPlot::DestroyContext();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}