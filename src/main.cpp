#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "implot.h"
#include "poligono.h"
#include "concreto.h"
#include <iostream>
#include <vector>
#include "reforco.h"

// Variáveis Globais
Reforco reforco;
Poligono poligono;
std::vector<Point> collectedPoints;
std::vector<Point> rotatedPoints = collectedPoints;
std::vector<Point> originalPoints = collectedPoints;

// Variáveis de Controle de Interface
float rotationAngle = 0.0f;
float keyDownDelay = 0.0f;
const float keyDownDelayTime = 0.1f;
float cuttingValue = 0.0f;
static int numVertices = 0;
static bool showDataWindow = true;
static bool showGraphWindow = true;

// Inicializa a Janela e o Contexto ImGui
void InitializeInterface() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Software de Cálculo de Esforços em Seções de Concreto Armado");

    if (!IsWindowReady()) {
        std::cerr << "Erro ao criar a janela!" << std::endl;
        return;
    }

    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
}

// Loop Principal
void ProgramLoop() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();
        DrawFPS(20, 20);
        keyDownDelay += GetFrameTime();

        // Janela de Dados
        if (showDataWindow) {
            ImGui::Begin("Inserir Dados", &showDataWindow);
            ImGui::Text("DADOS");
            ImGui::InputInt("Número de Pontos", &numVertices);

            if (numVertices != collectedPoints.size()) {
                collectedPoints.resize(numVertices);
            }

            // Tabela de Coordenadas de Pontos
            if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("x");
                ImGui::TableSetupColumn("y");
                ImGui::TableHeadersRow();

                for (int i = 0; i < collectedPoints.size(); i++) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::InputFloat(("x" + std::to_string(i)).c_str(), &collectedPoints[i].x);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputFloat(("y" + std::to_string(i)).c_str(), &collectedPoints[i].y);
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        // Janela de Gráfico
        if (showGraphWindow) {
            ImGui::Begin("Gráfico da Seção Transversal", &showGraphWindow);

            float x_values[2];
            float y_values[2] = {cuttingValue, cuttingValue};

            float minX = FLT_MAX, maxX = -FLT_MAX;
            for (const auto& point : rotatedPoints) {
                if (point.x > maxX) maxX = point.x;
                if (point.x < minX) minX = point.x;
            }

            x_values[0] = minX - (maxX - minX) * 0.1f;
            x_values[1] = maxX + (maxX - minX) * 0.1f;

            // Atualiza valor de corte e rotação com teclas
            if (keyDownDelay >= keyDownDelayTime) {
                if (IsKeyDown(KEY_UP)) cuttingValue += 1;
                if (IsKeyDown(KEY_DOWN)) cuttingValue -= 1;
                keyDownDelay = 0.0f;

                poligono.setVertices(collectedPoints);
                poligono.cortarPoligonal(poligono.verticesRotacionados, cuttingValue);
                rotatedPoints = poligono.verticesRotacionados;
            }

            if (IsKeyPressed(KEY_LEFT)) rotationAngle += 15;
            if (IsKeyPressed(KEY_RIGHT)) rotationAngle -= 15;

            // Atualiza a rotação dos pontos e barras de reforço
            poligono.setVertices(collectedPoints);
            poligono.translacaoCG(poligono.vertices);
            poligono.rotacao(rotationAngle);
            poligono.cortarPoligonal(poligono.verticesRotacionados, cuttingValue);
            rotatedPoints = poligono.verticesRotacionados;

            reforco.translacaoCG(reforco.Armaduras, poligono.centroide());
            reforco.RotacionarArmadura(rotationAngle);

            // Plotando Gráficos de Seções e Áreas
            if (ImPlot::BeginPlot("Gráfico", ImGui::GetContentRegionAvail())) {
                // Fechar e preparar as áreas superior e inferior para plotagem
                std::vector<Point> closedUpperArea = poligono.areaSuperior;
                std::vector<Point> closedLowerArea = poligono.areaInferior;

                poligono.fecharPoligono(closedUpperArea);
                poligono.fecharPoligono(closedLowerArea);

                // Convertendo pontos para arrays de float
                float x_upper[closedUpperArea.size()];
                float y_upper[closedUpperArea.size()];
                float x_lower[closedLowerArea.size()];
                float y_lower[closedLowerArea.size()];

                for (size_t i = 0; i < closedUpperArea.size(); i++) {
                    x_upper[i] = closedUpperArea[i].x;
                    y_upper[i] = closedUpperArea[i].y;
                }

                for (size_t i = 0; i < closedLowerArea.size(); i++) {
                    x_lower[i] = closedLowerArea[i].x;
                    y_lower[i] = closedLowerArea[i].y;
                }

                // Fechar e preparar pontos rotacionados para plotagem
                std::vector<Point> closedPolygon = rotatedPoints;
                poligono.fecharPoligono(closedPolygon);
                float x_rot[closedPolygon.size()];
                float y_rot[closedPolygon.size()];
                for (size_t i = 0; i < closedPolygon.size(); i++) {
                    x_rot[i] = closedPolygon[i].x;
                    y_rot[i] = closedPolygon[i].y;
                }

                // Plotando todas as seções e áreas
                ImPlot::PlotLine("Polígono Rotacionado", x_rot, y_rot, closedPolygon.size());
                ImPlot::PlotLine("Linha de Corte", x_values, y_values, 2);
                ImPlot::PlotLine("Área Superior", x_upper, y_upper, closedUpperArea.size());
                ImPlot::PlotLine("Área Inferior", x_lower, y_lower, closedLowerArea.size());
                
                ImPlot::EndPlot();
            }
            ImGui::End();
        }

        rlImGuiEnd();
        EndDrawing();
    }

    // Finalização do Contexto ImGui e Limpeza
    ImPlot::DestroyContext();
    rlImGuiShutdown();
}

// Função Principal
int main() {
    std::cout << "Iniciando a interface..." << std::endl;
    InitializeInterface();
    std::cout << "Entrando no loop do programa..." << std::endl;
    ProgramLoop();
    return 0;
}
