#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"
#include "implot.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include "data_storage.h"

// Classe Point para cálculos geométricos
struct Point {
    double x;
    double y;

    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

class Polygon {
private:
    std::vector<Point> vertices;

    bool isClockwise() const {
        double sum = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
        }
        return sum > 0;
    }

public:
    void setVertices(const std::vector<Ponto>& points) {
        vertices.clear();
        for (const auto& p : points) {
            vertices.emplace_back(p.x, p.y);
        }
    }

    void ensureCounterClockwise() {
        if (isClockwise()) {
            std::reverse(vertices.begin(), vertices.end());
        }
    }

    double area() const {
        double A = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            A += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
        }
        return std::abs(A) / 2.0;
    }

    Point centroid() const {
        double Cx = 0.0, Cy = 0.0;
        double A = area();
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            double factor = (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
            Cx += (vertices[i].x + vertices[j].x) * factor;
            Cy += (vertices[i].y + vertices[j].y) * factor;
        }
        Cx /= (6.0 * A);
        Cy /= (6.0 * A);
        return Point(Cx, Cy);
    }

    void printVertices() const {
        for (const auto& vertex : vertices) {
            std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
        }
    }

    // Método para cortar o polígono por uma linha horizontal
    std::vector<Point> cutByHorizontalLine(double y) {
        std::vector<Point> newVertices;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            Point p1 = vertices[i];
            Point p2 = vertices[(i + 1) % n];

            if (p1.y < y) {
                newVertices.push_back(p1);
            }
            if ((p1.y >= y && p2.y < y) || (p1.y < y && p2.y >= y)) {
                double x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                newVertices.push_back(Point(x, y));
            }
        }
        return newVertices;
    }

    // Método para cortar o polígono em uma posição específica
    void cut(double yPosition) {
        std::vector<Point> newVertices = cutByHorizontalLine(yPosition);
        if (!newVertices.empty()) {
            vertices = newVertices;
        }
    }
};

// Variáveis globais
std::vector<Ponto> collectedPoints; // Armazenar os pontos coletados
Polygon polygon;

// Funções de inicialização da interface
void IniciarInterface()
{
    const int screenWidth = 1280;
    const int screenHeight = 960;
    InitWindow(screenWidth, screenHeight, "Software de cálculo de esforços em concreto armado");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
}

void loopPrograma()
{
    static int tempNumPoints = 0;
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

            if (tempNumPoints != collectedPoints.size())
            {
                collectedPoints.resize(tempNumPoints); // Ajustar o tamanho do vetor
            }

            if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableHeadersRow();

                for (int row = 0; row < collectedPoints.size(); row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    char labelX[10], labelY[10];
                    snprintf(labelX, sizeof(labelX), "x%d", row);
                    snprintf(labelY, sizeof(labelY), "y%d", row);

                    ImGui::InputFloat(labelX, &collectedPoints[row].x);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputFloat(labelY, &collectedPoints[row].y);
                }

                ImGui::EndTable();
            }

            if (ImGui::Button("Mostrar Valores"))
            {
                TraceLog(LOG_INFO, "Valores armazenados:");
                for (const auto& point : collectedPoints)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }
            }

            if (ImGui::Button("Calcular Área e Centróide"))
            {
                polygon.setVertices(collectedPoints);  // Transfere os pontos para o polígono
                polygon.ensureCounterClockwise();      // Garante o sentido anti-horário

                double polygonArea = polygon.area();    // Calcula a área
                Point centroid = polygon.centroid();    // Calcula o centróide

                TraceLog(LOG_INFO, "Área: %.2f", polygonArea);
                TraceLog(LOG_INFO, "Centróide: (%.2f, %.2f)", centroid.x, centroid.y);
            }

            ImGui::End();  // Finaliza a janela de dados
        }

        // Janela do Gráfico
        if (showGraficoWindow) {
            ImGui::Begin("Gráfico da Seção Transversal", &showGraficoWindow);  // Título da janela

            static float x_data[100];
            static float y_data[100];
            int numPoints = collectedPoints.size();


            if (numPoints >= 3) // Verifica se foram inseridos pelo menos 3 vértices
            {
                // Preenche os arrays x_data e y_data com os pontos coletados
                for (int i = 0; i < numPoints; i++)
                {
                    x_data[i] = collectedPoints[i].x;
                    y_data[i] = collectedPoints[i].y;
                }

                // Adiciona o primeiro ponto ao final para fechar o polígono
                x_data[numPoints] = collectedPoints[0].x;
                y_data[numPoints] = collectedPoints[0].y;

                // Plota os pontos e desenha o polígono
                if (ImPlot::BeginPlot("Gráfico"))
                {
                    ImPlot::PlotScatter("Vértices", x_data, y_data, numPoints);
                    ImPlot::PlotLine("Polígono", x_data, y_data, numPoints + 1);  // Aumente para numPoints + 1
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

//não usou nenhuma função de data_storage
//collectedPoints começa com o quê? Vazio
//logo após comparamos o collectedPoits.size com tempNumPoints
//como sabemos o tamnho de collectedPoints se ele não foi declarado com nenhum ponto
//ImGui::InputFloat(labelX, &collectedPoints[row].x);
//ImGui::InputFloat(labelY, &collectedPoints[row].y);
//essas função são as responsáveis por atualizar a lista collectecPoints?
//debugar aqui para tentar entender
//verificar a possibilidade de utilizar a estrutura Point para exibir os vértices na interface
//talvez algo como referenciar o grafico já com as características bidimensionais 
//para evitar a abertura do colectedPoints de modo a armazenar em novas variáveis