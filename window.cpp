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

// static float VLN = 0; 

// Classe Point para cálculos geométricos
struct Point {
    float x;
    float y;

    Point(float x_val, float y_val) : x(x_val), y(y_val) {}
};

class Polygon {
/* private:
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
*/


public:
    std::vector<Point> vertices;
    void setVertices(const std::vector<Point>& points) {
        vertices.clear();
        for (const auto& p : points) {
            vertices.emplace_back(p.x, p.y);
        }
    }

    /* void ensureCounterClockwise() {
        if (isClockwise()) {
            std::reverse(vertices.begin(), vertices.end());
        }
    }*/

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
    
    // Método para cortar o polígono por uma linha horizontal
    void SP_Corte(int i, int j, float VLN, std::vector<Point>& newVertices) {
        double DELTAW = vertices[j].y - vertices[i].y;
        double DELTAU = vertices[j].x - vertices[i].x;

        if (std::abs(DELTAU) < 0.01) {
            return;
        }

        double COEFANG = DELTAW / DELTAU;
        double xIntersect = vertices[i].x - (vertices[i].y - VLN) / COEFANG;
        newVertices.emplace_back(xIntersect, VLN);
    }

    std::vector<Point> newVertices;

    std::vector<Point> cutByHorizontalLine(float VLN) {
        newVertices.clear();
        int NV = vertices.size();

        for (int i = 0; i < NV; i++) {
            int j = (i + 1) % NV;


            if(vertices[i].y <= VLN && vertices[j].y >= VLN){
                SP_Corte(i, j, VLN, newVertices);
            }

            if (vertices[i].y >= VLN && vertices[j].y <= VLN) {
                SP_Corte(i, j, VLN, newVertices);
            }

            if (vertices[i].y >= VLN && vertices[j].y >= VLN) {
                newVertices.emplace_back(vertices[i].x, vertices[i].y);
            }
        }

        return newVertices;
        // ensureCounterClockwise();
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

    std::vector<Point> cortarPoligonal(const std::vector<Point>& vertices, const std::vector<float>& cortar) {
        std::vector<Point> resultado = vertices;

        for (float nivel : cortar) {
            std::vector<Point> novaPoligonal;
            int nv = resultado.size();

            for (int i = 0; i < nv; i++) {
                int caso = verificarCaso(resultado[i], resultado[(i + 1) % nv], nivel);

                if (caso == 2) {
                    Point intersecao = calcularIntersecao(resultado[i], resultado[(i + 1) % nv], nivel);
                    novaPoligonal.push_back(intersecao);
                }

                novaPoligonal.push_back(resultado[(i + 1) % nv]);
            }

            resultado = novaPoligonal;
        }

        return resultado;
}


};


/*

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

    std::vector<Point> cortarPoligonal(const std::vector<Point>& collectedPoints, const std::vector<double>& cortar) {
        std::vector<Point> resultado = secao;

        for (double nivel : cortar) {
            std::vector<Point> novaPoligonal;
            int nv = resultado.size();

            for (int i = 0; i < nv; i++) {
                int caso = verificarCaso(resultado[i], resultado[(i + 1) % nv], nivel);

                if (caso == 2) {
                    Point intersecao = calcularIntersecao(resultado[i], resultado[(i + 1) % nv], nivel);
                    novaPoligonal.push_back(intersecao);
                }

                novaPoligonal.push_back(resultado[(i + 1) % nv]);
            }

            resultado = novaPoligonal;
        }

        return resultado;
}

*/

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
                ImGui::Text("Insira a coordenada Y do corte");

                int NC = um temp num poins da qtd de cortes

                for(int i = 0; i < NC; i++){
                    ImGui::InputFloat("Y", &VLN);
                    cortar.push_back(VLN);
                }

            
            if (ImGui::Button("Cortar"))
            {
                polygon.cutByHorizontalLine(VLN);
                int NV = polygon.newVertices.size();

                TraceLog(LOG_INFO, "Valores Armazenados do Corte");
                for (int i = 0; i <= NV; i++){
                    TraceLog(LOG_INFO, "x = %2.f, y = %2.f", polygon.newVertices[i].x, polygon.newVertices[i].y);
                }
            };

            if(ImGui::Button("Cortar2")){
                polygon.cortarPoligonal(polygon.vertices, cortar);
                int NV = 
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