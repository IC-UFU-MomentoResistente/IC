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
#include <set>

struct Point
{
    float x;
    float y;

    Point() : x(0), y(0) {}
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}

    // Definir o operador de comparação para que o std::set funcione
    bool operator<(const Point &other) const
    {
        if (x != other.x)
        {
            return x < other.x;
        }
        return y < other.y;
    }
};

class Polygon
{
public:
    std::vector<Point> vertices;
    std::vector<Point> resultadoCorte;
    std::vector<Point> AreaSuperior;
    std::vector<Point> AreaInferior;

    void setVertices(const std::vector<Point> &points)
    {
        vertices.clear();
        for (const auto &p : points)
        {
            vertices.emplace_back(p.x, p.y);
        }
    }

    int verificarCaso(const Point &p1, const Point &p2, double cortar)
    {
        if ((p2.y <= cortar && p1.y >= cortar) || (p2.y >= cortar && p1.y <= cortar))
        {
            return 2; // Caso 2: Corta o trecho
        }
        if (p2.y >= cortar && p1.y >= cortar)
        {
            return 3; // caso 3: vértices acima da linha de corte; área comprimida
        }
        if (p2.y <= cortar && p1.y <= cortar)
        {
            return 4; // caso 4: vértices abaixo da linha de corte: área tracionada
        }
        return 1; // Caso 1: Adiciona o nó final na nova poligonal
    }

    // Função para calcular o ponto de interseção
    Point calcularIntersecao(const Point &p1, const Point &p2, double cortar)
    {
        double deltaY = p2.y - p1.y;
        double deltaX = p2.x - p1.x;

        if (std::abs(deltaX) < 1e-6)
        {
            return Point(p1.x, cortar);
        }
        else
        {
            double tg = deltaY / deltaX;
            double x = p1.x - (p1.y - cortar) / tg;
            return Point(x, cortar);
        }
    }

    void cortarPoligonal(const std::vector<Point> &vertices, float &cortar)
    {
        resultadoCorte.clear();
        AreaSuperior.clear();
        AreaInferior.clear();
        int nv = vertices.size();
        std::set<Point> pontosAdicionados; // Conjunto para rastrear pontos já adicionados

        for (int i = 0; i < nv; i++)
        {
            int caso = verificarCaso(vertices[i], vertices[(i + 1) % nv], cortar);

            if (caso == 2)
            {
                Point intersecao = calcularIntersecao(vertices[i], vertices[(i + 1) % nv], cortar);
                if (pontosAdicionados.insert(intersecao).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(intersecao);
                    AreaSuperior.push_back(intersecao);
                    AreaInferior.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    AreaSuperior.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    AreaSuperior.push_back(vertices[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    AreaInferior.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    AreaInferior.push_back(vertices[(i + 1) % nv]);
                }
            }
        }
    }

    void fecharPoligono(std::vector<Point> &pontos)
    {
        if (!pontos.empty())
        {
            pontos.push_back(pontos[0]);
        }
    }
};

// Variáveis globais
std::vector<Point> collectedPoints = {
    {0,190}, {0,178}, {50,170}, {50,45}, {25,25}, {25,0},
    {95,0}, {95, 25}, {70,45}, {70,170}, {120,178}, {120,190}
}; // Armazenar os pontos coletados
Polygon polygon;

// Funções de inicialização da interface
void IniciarInterface()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Software de cálculo de esforços em seções de concreto armado");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
}

void loopPrograma()
{
    float KeyDownDelay = 0.0f;
    float KeyDownDelayTime = 0.1f;

    int numBarras = 1;
    static float VLN = 0;
    static float cortar = 0;
    static float diametroBarras = 0;
    static float diametroPosXi = 0;
    static float diametroPosYi = 0;
    static float diametroPosXf = 0;
    static float diametroPosYf = 0;
    static bool showGraficoWindow = true;
    static bool showDadosWindowTwo = true;
    static bool tabelaArmadura = true;
    static bool umaBarra = false;
    static bool linhaBarras = false;

    while (!WindowShouldClose())
    {
        DrawFPS(20, 20);
        KeyDownDelay = KeyDownDelay + GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        if (showDadosWindowTwo)
        {
            ImGui::Begin("Central de operações com polígono", &showDadosWindowTwo);
            ImGui::Text("Insira a coordenada de corte");

            // cortar.clear(); // Limpa o vetor de cortes antes de adicionar novos valores

            // Declare VLN fora do loop, se ainda não estiver declarado
            // Usar static para manter o valor entre as chamadas
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Y", &VLN); // Permite ao usuário inserir a coordenada Y

            // Adiciona a coordenada Y ao vetor de cortes
            if (ImGui::Button("Adicionar Corte"))
            { // Botão para adicionar o corte
                cortar = VLN;
            }

            if (ImGui::Button("Cortar"))
            {
                polygon.setVertices(collectedPoints);
                polygon.cortarPoligonal(polygon.vertices, cortar);
            }

            if (ImGui::Button("Mostrar Valores"))
            {

                TraceLog(LOG_INFO, "Valores armazenados:");
                for (const auto &point : collectedPoints)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Valores após o corte");
                for (const auto &point : polygon.resultadoCorte)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Vertices superiores");
                for (const auto &point : polygon.AreaSuperior)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Vertices inferiores");
                for (const auto &point : polygon.AreaInferior)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }
            }

            ImGui::End();
        }

        if (tabelaArmadura)
        {
            ImGui::Begin("Entrada de dados: Armadura Passiva", &tabelaArmadura);
            ImGui::Checkbox("Uma Barra", &umaBarra);
            if (umaBarra == true)
            {
                linhaBarras = false;
            }

            ImGui::Checkbox("Linha de Barras", &linhaBarras);
            if (linhaBarras == true)
            {
                umaBarra = false;
            }

            if (umaBarra == true)
            {
                numBarras = 1;
                ImGui::PushItemWidth(50);
                ImGui::InputFloat("Diâmetro das Barras", &diametroBarras);
                ImGui::InputFloat("Posição X (mm)", &diametroPosXi);
                ImGui::SameLine();
                ImGui::InputFloat("Posição Y (mm)", &diametroPosYi);
            }

            if (linhaBarras == true)
            {
                ImGui::SetNextItemWidth(100);
                ImGui::InputInt("Numero de Barras na Linha", &numBarras);
                ImGui::PushItemWidth(50);
                ImGui::InputFloat("Diâmetro das Barras", &diametroBarras);
                ImGui::InputFloat("Posição Xi (mm)", &diametroPosXi);
                ImGui::SameLine();
                ImGui::InputFloat("Posição Yi (mm)", &diametroPosYi);
                ImGui::InputFloat("Posição Xf (mm)", &diametroPosXf);
                ImGui::SameLine();
                ImGui::InputFloat("Posição Yf (mm)", &diametroPosYf);
            }
            ImGui::Button("Adicionar");
            ImGui::SameLine();
            ImGui::Button("Remover");
            if (ImGui::BeginTable("Tabela", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {

                ImGui::TableSetupColumn("Barra");
                ImGui::TableSetupColumn("Diâmetro");
                ImGui::TableSetupColumn("X(cm)");
                ImGui::TableSetupColumn("Y(cm)");
                ImGui::TableHeadersRow();
                ImGui::EndTable();
            }

            ImGui::End();
        }

        if (showGraficoWindow)
        {
            ImGui::Begin("Gráfico da Seção Transversal", &showGraficoWindow); // Título da janela

            static float x_values[2]; // teste
            static float y_values[2];

            float valorMenor = FLT_MAX;  // Número de ponto flutuante representável máximo.
            float valorMaior = -FLT_MAX; // Número de ponto flutuante representável máximo.

            for (int i = 0; i < sizeof(x_values); i++)
            {
                y_values[i] = VLN;
            }

            if (IsKeyPressed(KEY_UP))
            {
                VLN = VLN + 1;

                cortar = VLN;
                polygon.setVertices(collectedPoints);
                polygon.cortarPoligonal(polygon.vertices, cortar);

                KeyDownDelay = 0.0f;
            }
            if (IsKeyPressed(KEY_DOWN))
            {
                VLN = VLN - 1;

                cortar = VLN;
                polygon.setVertices(collectedPoints);
                polygon.cortarPoligonal(polygon.vertices, cortar);

                KeyDownDelay = 0.0f;
            }

            if (KeyDownDelay >= KeyDownDelayTime)
            {
                if (IsKeyDown(KEY_UP))
                {

                    VLN = VLN + 1;
                    cortar = VLN;
                    polygon.setVertices(collectedPoints);
                    polygon.cortarPoligonal(polygon.vertices, cortar);

                    KeyDownDelay = 0.0f;
                }
                if (IsKeyDown(KEY_DOWN))
                {
                    VLN = VLN + -1;
                    cortar = VLN;
                    polygon.setVertices(collectedPoints);
                    polygon.cortarPoligonal(polygon.vertices, cortar);

                    KeyDownDelay = 0.0f;
                }
            }

            int numPoints = collectedPoints.size();

            for (int i = 0; i <= numPoints; i++)
            {
                if (collectedPoints[i].x > valorMaior)
                {
                    valorMaior = collectedPoints[i].x;
                }

                if (collectedPoints[i].x < valorMenor)
                {
                    valorMenor = collectedPoints[i].x;
                }
            }

            x_values[0] = valorMenor - (valorMaior - valorMenor) * 0.1;
            x_values[1] = valorMaior + (valorMaior - valorMenor) * 0.1;

            // Ajuste o código de desenho no gráfico
            if (numPoints >= 3)
            {
                // Fechar os vetores adicionando o primeiro ponto ao final
                std::vector<Point> collectedPointsFechados = collectedPoints;
                polygon.fecharPoligono(collectedPointsFechados);
                std::vector<Point> resultadoCorteFechado = polygon.resultadoCorte;
                polygon.fecharPoligono(resultadoCorteFechado);
                std::vector<Point> AreaSuperiorFechado = polygon.AreaSuperior;
                polygon.fecharPoligono(AreaSuperiorFechado);
                std::vector<Point> AreaInferiorFechado = polygon.AreaInferior;
                polygon.fecharPoligono(AreaInferiorFechado);

                // Converter para arrays de float para os gráficos
                float x_data[collectedPointsFechados.size()];
                float y_data[collectedPointsFechados.size()];
                float x_corte[resultadoCorteFechado.size()];
                float y_corte[resultadoCorteFechado.size()];
                float x_superior[AreaSuperiorFechado.size()];
                float y_superior[AreaSuperiorFechado.size()];
                float x_inferior[AreaInferiorFechado.size()];
                float y_inferior[AreaInferiorFechado.size()];

                for (size_t i = 0; i < collectedPointsFechados.size(); i++)
                {
                    x_data[i] = collectedPointsFechados[i].x;
                    y_data[i] = collectedPointsFechados[i].y;
                }

                for (size_t i = 0; i < resultadoCorteFechado.size(); i++)
                {
                    x_corte[i] = resultadoCorteFechado[i].x;
                    y_corte[i] = resultadoCorteFechado[i].y;
                }

                for (size_t i = 0; i < AreaSuperiorFechado.size(); i++)
                {
                    x_superior[i] = AreaSuperiorFechado[i].x;
                    y_superior[i] = AreaSuperiorFechado[i].y;
                }

                for (size_t i = 0; i < AreaInferiorFechado.size(); i++)
                {
                    x_inferior[i] = AreaInferiorFechado[i].x;
                    y_inferior[i] = AreaInferiorFechado[i].y;
                }

                // Obter o tamanho disponível para o gráfico
                ImVec2 plotSize = ImGui::GetContentRegionAvail();

                // Plota os pontos e desenha o polígono
                if (ImPlot::BeginPlot("Gráfico", ImVec2(plotSize.x, plotSize.y)))
                {
                    ImPlot::PlotScatter("Vértices", x_data, y_data, collectedPointsFechados.size());
                    ImPlot::PlotScatter("Vértices cortadas", x_corte, y_corte, resultadoCorteFechado.size());
                    ImPlot::PlotScatter("Vértices superiores", x_superior, y_superior, AreaSuperiorFechado.size());
                    ImPlot::PlotScatter("Vértices inferiores", x_inferior, y_inferior, AreaInferiorFechado.size());
                    ImPlot::PlotLine("Polígono", x_data, y_data, collectedPointsFechados.size());
                    ImPlot::PlotLine("Polígono cortado", x_corte, y_corte, resultadoCorteFechado.size());
                    ImPlot::PlotLine("Polígono superior", x_superior, y_superior, AreaSuperiorFechado.size());
                    ImPlot::PlotLine("Polígono inferior", x_inferior, y_inferior, AreaInferiorFechado.size());
                    ImPlot::PlotLine("Linha de corte", x_values, y_values, 2);
                    ImPlot::EndPlot();
                }
            }
            else
            {
                ImGui::Text("Insira pelo menos 3 vértices para formar um polígono.");
            }

            ImGui::End(); // Finaliza a janela do gráfico
        }

        rlImGuiEnd();
        EndDrawing();
    }

    // Finaliza o contexto
    ImPlot::DestroyContext();
    rlImGuiShutdown();
}

// Tarefas
// Definir seção I pronta para começar a interface FEITO
// Inserir pontos de corte na janela do gráfico  FEITO
// Ajustar tamanho dos vetores x_data, y_data, x_corte, y_corte FEITO
// Ajustar desenho da linha com os limites de x + 10% FEITO
// Funções para corte superior e corte inferior FEITO
// Corta com as setas do teclado a cada frame ou a cada "clique" FEITO
// Rodar a seção com as setas - A FAZER