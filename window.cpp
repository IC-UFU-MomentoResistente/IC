#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"
#include "implot.h"
#include "data_storage.h" // Inclui a biblioteca de armazenamento de dados
#include <stdio.h>
#include "window.h"
#include <cmath>
#include <vector>
#include <algorithm>

// Estrutura para armazenar os pontos
struct Ponto {
    float x, y;
};

// Função para calcular o centroide
Ponto CalcularCentroide(const std::vector<Ponto>& pontos) {
    Ponto centroide = {0.0f, 0.0f};
    for (const auto& ponto : pontos) {
        centroide.x += ponto.x;
        centroide.y += ponto.y;
    }
    centroide.x /= pontos.size();
    centroide.y /= pontos.size();
    return centroide;
}

// Função para calcular o ângulo polar
float CalcularAngulo(const Ponto& centroide, const Ponto& ponto) {
    return atan2(ponto.y - centroide.y, ponto.x - centroide.x);
}

// Função para ordenar os pontos com base no ângulo polar
void OrdenarPontosPorAngulo(std::vector<Ponto>& pontos) {
    Ponto centroide = CalcularCentroide(pontos);
    std::sort(pontos.begin(), pontos.end(), [&](const Ponto& a, const Ponto& b) {
        return CalcularAngulo(centroide, a) < CalcularAngulo(centroide, b);
    });
}

void IniciarInterface()
{
    const int screenWidth = 1280;
    const int screenHeight = 960;
    InitWindow(screenWidth, screenHeight, "Software de cálculo de esforços em concreto armado");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
    InitData();
}

void loopPrograma()
{
    static int tempNumPoints = GetNumPoints();
    static bool showGraficoWindow = true;
    static bool showDadosWindow = true;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        // Janela de Inserção de Dados
        if (showDadosWindow) {
            ImGui::Begin("Inserir Dados", &showDadosWindow);  // Título da janela

            ImGui::Text("DADOS");
            ImGui::InputInt("Número de Pontos", &tempNumPoints);

            if (tempNumPoints != GetNumPoints())
            {
                SetNumPoints(tempNumPoints);
            }

            if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableHeadersRow();

                for (int row = 0; row < GetNumPoints(); row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    char labelX[10], labelY[10];
                    snprintf(labelX, sizeof(labelX), "x%d", row);
                    snprintf(labelY, sizeof(labelY), "y%d", row);

                    float x, y;
                    GetTableData(row, &x, &y);

                    ImGui::InputFloat(labelX, &x);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputFloat(labelY, &y);

                    SetTableData(row, x, y);
                }

                ImGui::EndTable();
            }

            if (ImGui::Button("Mostrar Valores"))
            {
                TraceLog(LOG_INFO, "Valores armazenados:");
                for (int row = 0; row < GetNumPoints(); row++)
                {
                    float x, y;
                    GetTableData(row, &x, &y);
                    TraceLog(LOG_INFO, "Linha %d: x = %.2f, y = %.2f", row + 1, x, y);
                }
            }

            ImGui::End();  // Finaliza a janela de dados
        }

        // Janela do Gráfico
        if (showGraficoWindow) {
            ImGui::Begin("Gráfico da Seção Transversal", &showGraficoWindow);  // Título da janela

            static float x_data[100];
            static float y_data[100];
            int numPoints = GetNumPoints();

            // Vetor para armazenar os pontos
            std::vector<Ponto> pontos;
            for (int i = 0; i < numPoints; i++)
            {
                Ponto p;
                GetTableData(i, &p.x, &p.y); // Pegue os dados da tabela
                pontos.push_back(p);
            }

            if (numPoints >= 3) // Verifica se foram inseridos pelo menos 3 vértices
            {
                // Ordena os pontos para evitar cruzamentos
                OrdenarPontosPorAngulo(pontos);

                // Adiciona o primeiro ponto ao final para fechar o polígono
                pontos.push_back(pontos[0]);

                // Preenche os arrays x_data e y_data com os pontos ordenados
                for (int i = 0; i < pontos.size(); i++) {
                    x_data[i] = pontos[i].x;
                    y_data[i] = pontos[i].y;
                }

                // Plota os pontos e desenha o polígono
                if (ImPlot::BeginPlot("Gráfico"))
                {
                    ImPlot::PlotScatter("Vértices", x_data, y_data, numPoints);
                    ImPlot::PlotLine("Polígono", x_data, y_data, numPoints + 1);
                    ImPlot::EndPlot();
                }
            }
            else
            {
                ImGui::Text("Insira pelo menos 3 vértices para formar um polígono.");
            }

            ImGui::End();  // Finaliza a janela do gráfico
        }

        rlImGuiEnd();
        EndDrawing();
    }

    // Finaliza o contexto
    ImPlot::DestroyContext();
    rlImGuiShutdown();
}
