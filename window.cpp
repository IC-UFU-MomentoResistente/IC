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

struct Point {
    float x;
    float y;

    Point() : x(0), y(0) {}
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}
};

class Polygon {
public:
    std::vector<Point> vertices;
    void setVertices(const std::vector<Point>& points) {
        vertices.clear();
        for (const auto& p : points) {
            vertices.emplace_back(p.x, p.y);
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
    
    int verificarCaso(const Point& p1, const Point& p2, double cortar) {
        if ((p2.y <= cortar && p1.y >= cortar) || (p2.y >= cortar && p1.y <= cortar)) {
            return 2; // Caso 2: Corta o trecho
        }
        return 1; // Caso 1: Adiciona o nó final na nova poligonal
    }

    // Função para calcular o ponto de interseção
    Point calcularIntersecao(const Point& p1, const Point& p2, double cortar) {
        double deltaY = p2.y - p1.y;
        double deltaX = p2.x - p1.x;
        
        if (std::abs(deltaX) < 1e-6) {
            return Point(p1.x, cortar);
        } else {
            double tg = deltaY / deltaX;
            double x = p1.x - (p1.y - cortar) / tg;
            return Point(x, cortar);
        }
    }

    std::vector<Point> resultadoCorte = vertices;
    std::vector<Point> cortarPoligonal(const std::vector<Point>& vertices, const std::vector<float>& cortar) {

        for (float nivel : cortar) {
            std::vector<Point> novaPoligonal;
            int nv = resultadoCorte.size();

            for (int i = 0; i < nv; i++) {
                int caso = verificarCaso(resultadoCorte[i], resultadoCorte[(i + 1) % nv], nivel);

                if (caso == 2) {
                    Point intersecao = calcularIntersecao(resultadoCorte[i], resultadoCorte[(i + 1) % nv], nivel);
                    novaPoligonal.push_back(intersecao);
                }

                novaPoligonal.push_back(resultadoCorte[(i + 1) % nv]);
            }
            resultadoCorte = novaPoligonal;
        }
        return resultadoCorte;
    }
};


// Variáveis globais
std::vector<Point> collectedPoints; // Armazenar os pontos coletados
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
    float VLN = 0;
    std::vector<float> cortar;
    static int tempNumPoints = 0;
    static bool showGraficoWindow = true;
    static bool showDadosWindow = true;
    static bool showDadosWindowTwo = true; 
    
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

                       
            ImGui::End();  // Finaliza a janela de dados
        }

            if (showDadosWindowTwo) {
                ImGui::Begin("Central de operações com polígono", &showDadosWindowTwo);
                ImGui::Text("Insira a coordenada de corte");

                // cortar.clear(); // Limpa o vetor de cortes antes de adicionar novos valores

                // Declare VLN fora do loop, se ainda não estiver declarado
                static float VLN = 0.0f; // Usar static para manter o valor entre as chamadas

                ImGui::InputFloat("Y", &VLN); // Permite ao usuário inserir a coordenada Y

                // Adiciona a coordenada Y ao vetor de cortes
                if (ImGui::Button("Adicionar Corte")) { // Botão para adicionar o corte
                    cortar.push_back(VLN);
                }
                
                ImGui::Text("Cortes adicionados:");
                for (const auto& corte : cortar) {
                        ImGui::Text("%.2f", corte); // Mostra cada corte adicionado
                }


                if(ImGui::Button("Cortar")){
                    polygon.setVertices(collectedPoints);
                    polygon.cortarPoligonal(polygon.vertices, cortar);
                    int NV = polygon.vertices.size();
                    TraceLog(LOG_INFO, "Valores dos Cortes Armazenados");
                    for(int i = 0; i < NV; i++){
                        TraceLog(LOG_INFO, "x = %2.f, y = %2.f", polygon.resultadoCorte[i].x, polygon.resultadoCorte[i].y);
                    }
                }
            
            if (ImGui::Button("Mostrar Valores"))
            {
                TraceLog(LOG_INFO, "VLN", VLN);
                TraceLog(LOG_INFO, "Valor do corte", &cortar[0]);
                TraceLog(LOG_INFO, "Valores armazenados:");
                for (const auto& point : collectedPoints)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);             
                }
            }
            
            if (ImGui::Button("Calcular Área e Centróide"))
            {
                polygon.setVertices(collectedPoints);  // Transfere os pontos para o polígono
                 // Garante o sentido anti-horário

                double polygonArea = polygon.area();    // Calcula a área
                Point centroid = polygon.centroid();    // Calcula o centróide

                TraceLog(LOG_INFO, "Área: %.2f", polygonArea);
                TraceLog(LOG_INFO, "Centróide: (%.2f, %.2f)", centroid.x, centroid.y);

            }

                                 
                ImGui::End();
            
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



// Colocar uma caixa de texto para o usuário inserir a coordenada y de corte
// Botão para mostrar a linha de corte antes de cortar

// Botão que corta o polígono

// Janela que mostra o polígono comprimido