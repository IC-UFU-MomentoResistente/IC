#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "implot.h"
#include "poligono.h"
#include "concreto.h"
#include <iostream>
#include <vector>
#include "reforco.h"

Reforco reforco;

std::vector<Point> collectedPoints = {
    {-10, -20}, {10, -20}, {10, 20}, {-10, 20}}; // Armazenar os pontos coletados

Poligono poligono;
std::vector<Point> Rot = collectedPoints;
std::vector<Point> pontosOriginais = collectedPoints;

float graus = 0;
Point centroideInicial;

/* Point calcularCentroide(const std::vector<Point> &pontos)
{
    float somaX = 0.0f;
    float somaY = 0.0f;
    int n = pontos.size();

    for (const auto &p : pontos)
    {
        somaX += p.x;
        somaY += p.y;
    }

    return Point(somaX / n, somaY / n);
}
*/ 
/* void rotacionarPoligono(std::vector<Point> &collectedPoints)
{
    for (size_t i = 0; i < pontosOriginais.size(); ++i)
    {
        // transladar os pontos para a origem do centróide
        double xTransladado = pontosOriginais[i].x - centroideInicial.x;
        double yTransladado = pontosOriginais[i].y - centroideInicial.y;

        // Aplicar a rotação
        double xRotacionado = xTransladado * cos(radianos) - yTransladado * sin(radianos);
        double yRotacionado = xTransladado * sin(radianos) + yTransladado * cos(radianos);

        // Transladar os pontos de volta para a posição original em relação ao centróide
        poligono.verticesRotacionados[i].x;
        
        collectedPoints[i].x = xRotacionado + centroideInicial.x;
        collectedPoints[i].y = yRotacionado + centroideInicial.y;

        Rot[i].x = xRotacionado + centroideInicial.x;
        Rot[i].y = yRotacionado + centroideInicial.y;
    }
}
*/ 
void IniciarInterface()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Software de cálculo de esforços em seções de concreto armado");

    if (!IsWindowReady()) // Verifique se a janela foi criada com sucesso
    {
        std::cerr << "Erro ao criar a janela!" << std::endl;
        return; // Saia da função se a janela não foi criada
    }

    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
    // centroideInicial = calcularCentroide(collectedPoints);
}


float KeyDownDelay = 0.0f;
float KeyDownDelayTime = 0.1f;
int numBarras = 0;
int barras = 0;
static float VLN = 0;
static float cortar = 0;
static float diametroBarras = 0;
static float barrasPosXi = 0;
static float barrasPosYi = 0;
static float barrasPosXf = 0;
static float barrasPosYf = 0;
static bool janelaGrafico = true;
static bool janelaPoligono = true;
static bool tabelaArmadura = true;
static bool janelaConcreto = true;

void loopPrograma()
{
    while (!WindowShouldClose())
    {
        DrawFPS(20, 20);
        KeyDownDelay = KeyDownDelay + GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        if (janelaPoligono)
        {
            ImGui::Begin("Central de operações com polígono", &janelaPoligono);
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

            if (ImGui::Button("Mostrar Valores"))
            {

                TraceLog(LOG_INFO, "Valores armazenados:");
                for (const auto &point : collectedPoints)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Valores após o corte");
                for (const auto &point : poligono.resultadoCorte)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Vertices superiores");
                for (const auto &point : poligono.areaSuperior)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Vertices inferiores");
                for (const auto &point : poligono.areaInferior)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }
            }

            if (ImGui::Button("Calcular Área e Centróide"))
            {
                poligono.setVertices(collectedPoints); // Transfere os pontos para o polígono
                double poligonoArea = poligono.area();  // Calcula a área
                Point centroide = poligono.centroide();  // Calcula o centróide

                TraceLog(LOG_INFO, "Área: %.2f", poligonoArea);
                TraceLog(LOG_INFO, "Centróide: (%.2f, %.2f)", centroide.x, centroide.y);
            }

            ImGui::End();
        }

        static float fck = 0;
        static float gama_c = 0;
        static float eps1 = 0;
        static float eps2 = 0;
        static float x_d = 0;
        static float d = 0;
        static float fyk = 0;
        static float gama_s = 0;
        static float Es = 0;
        static float Epap = 0;
        static float EpA = 0;

        if (janelaConcreto)
        {
            ImGui::Begin ("Entradas de dados: Parâmetros Concreto", &janelaConcreto);
            ImGui::Text("Insira os valores de fck e gama_c");
            
            ImGui::InputFloat("fck", &fck);
            ImGui::InputFloat("gama_c", &gama_c);
            ImGui::InputFloat("eps1", &eps1);
            ImGui::InputFloat("eps2", &eps2);
            ImGui::InputFloat("x sobre d", &x_d);
            ImGui::InputFloat("d", &d);
            ImGui::InputFloat("fyk", &fyk);
            ImGui::InputFloat("gama_S", &gama_s);
            ImGui::InputFloat("Es", &Es);
            ImGui::InputFloat("Epap", &Epap);
            ImGui::InputFloat("EpA", &EpA);
            
            if (ImGui::Button("Calcular parâmetros"))
            {
                Concreto concreto (fck, gama_c, eps1, eps2, x_d, d);
                Concreto::ParametrosConcreto parametrosConcreto = concreto.getParametros();
                Concreto::AlturasConcreto alturasConcreto = concreto.getAlturas();
                reforco.calculaParametros(fyk, gama_s, Es);
                reforco.calculaNormal_Momento(Epap, EpA);

                TraceLog(LOG_INFO, "Parâmetros do Concreto Calculados");
                TraceLog(LOG_INFO, "Fator multiplicativo: %.2f", parametrosConcreto.fatorMultTensaoCompConcreto);
                TraceLog(LOG_INFO, "Ep ultimo: %.2f", parametrosConcreto.epsilonConcretoUltimo);
                TraceLog(LOG_INFO, "Ep2: %.2f", parametrosConcreto.epsilonConcreto2);
                TraceLog(LOG_INFO, "Expoente: %.2f", parametrosConcreto.expoenteTensaoConcreto);
                TraceLog(LOG_INFO, "fcd: %.2f", parametrosConcreto.fcd);
                TraceLog(LOG_INFO, "altura 2/1000: %.2f", alturasConcreto.altura_deformacao_2);
                TraceLog(LOG_INFO, "altuara ultima: %.2f", alturasConcreto.altura_deformacao_ultima);
                TraceLog(LOG_INFO, "altura LN: %.2f", alturasConcreto.altura_LN);
                TraceLog(LOG_INFO, "fyd: %.2f", reforco.fyd);
                TraceLog(LOG_INFO, "epyd: %.7f", reforco.epsilon_yd);
                TraceLog(LOG_INFO, "Normal Aço Passivo: %.4f", reforco.soma_normal_aco_passivo);
                TraceLog(LOG_INFO, "Momento Aço Passivo: %.4f", reforco.soma_momento_aco_passivo);
                
            }

            ImGui::End();
        }

        if (tabelaArmadura)
        {
            ImGui::Begin("Entrada de dados: Armadura Passiva", &tabelaArmadura);
            ImGui::RadioButton("Uma Barra", &barras, 0);
            ImGui::RadioButton("Linha de Barras", &barras, 1);

            if (barras == 0)
            {
                numBarras = 1;
                ImGui::PushItemWidth(50);
                ImGui::InputFloat("Diâmetro das Barras", &diametroBarras);
                ImGui::InputFloat("Posição X (mm)", &barrasPosXi);
                ImGui::InputFloat("Posição Y (mm)", &barrasPosYi);
                
                if (ImGui::Button("Adicionar"))
                {
                    if(diametroBarras <= 0) 
                    {

                    }
                    else {
                    reforco.AdicionarBarra(barrasPosXi, barrasPosYi, diametroBarras);
                    Point centroide = poligono.centroide();

                    reforco.translacaoCG(reforco.Armaduras, centroide);
                    reforco.RotacionarArmadura(graus);
                    }
                }

                if (ImGui::Button("Remover"))
                {
                    reforco.RemoverBarra();
                    Point centroide = poligono.centroide();

                    reforco.translacaoCG(reforco.Armaduras, centroide);
                    reforco.RotacionarArmadura(graus);
                }
            }

            if (barras == 1)
            {
                ImGui::SetNextItemWidth(100);
                ImGui::InputInt("Numero de Barras na Linha", &numBarras);
                if (numBarras < 2)
                {
                    numBarras = 2;
                }
                ImGui::PushItemWidth(50);
                ImGui::InputFloat("Diâmetro das Barras", &diametroBarras);
                ImGui::InputFloat("Posição Xi (mm)", &barrasPosXi);
                ImGui::InputFloat("Posição Yi (mm)", &barrasPosYi);
                ImGui::InputFloat("Posição Xf (mm)", &barrasPosXf);
                ImGui::InputFloat("Posição Yf (mm)", &barrasPosYf);

                float xAdicionado = (barrasPosXf - barrasPosXi) / (numBarras - 1);
                float yAdicionado = (barrasPosYf - barrasPosYi) / (numBarras - 1);
                float valorAdicionadoX = 0;
                float valorAdicionadoY = 0;
    
                if (ImGui::Button("Adicionar"))
                {
                    if(diametroBarras <= 0) 
                    {

                    }
                    else {
                    for (int i = 0; i < numBarras; i++)
                    {
                        valorAdicionadoX = barrasPosXi + xAdicionado * i;
                        valorAdicionadoY = barrasPosYi + yAdicionado * i;

                        reforco.AdicionarBarra(valorAdicionadoX, valorAdicionadoY, diametroBarras);
                        Point centroide = poligono.centroide();

                        reforco.translacaoCG(reforco.Armaduras, centroide);
                        reforco.RotacionarArmadura(graus);
                    }
                    }
                }
                if (ImGui::Button("Remover"))
                {
                    reforco.RemoverBarra();
                    Point centroide = poligono.centroide();

                    reforco.translacaoCG(reforco.Armaduras, centroide);
                    reforco.RotacionarArmadura(graus);
                }
            }

            if (ImGui::BeginTable("Tabela", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Posição X (mm)");
                ImGui::TableSetupColumn("Posição Y (mm)");
                ImGui::TableSetupColumn("Diâmetro");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < reforco.Armaduras.size(); ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", static_cast<int>(i + 1));
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", reforco.Armaduras[i].x);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%.2f", reforco.Armaduras[i].y);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.2f", reforco.valorDiametroBarras[i]);
                }
                ImGui::EndTable();
            }

            ImGui::End();
        }

        /* if (janelaGrafico)
        {
            ImGui::Begin("Gráfico da Seção Transversal", &janelaGrafico); // Título da janela

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
                poligono.setVertices(collectedPoints);
                poligono.cortarPoligonal(poligono.verticesRotacionados, cortar);
                Rot = poligono.verticesRotacionados;
                KeyDownDelay = 0.0f;
            }
            if (IsKeyPressed(KEY_DOWN))
            {
                VLN = VLN - 1;

                cortar = VLN;
                poligono.setVertices(collectedPoints);
                poligono.cortarPoligonal(poligono.verticesRotacionados, cortar);
                Rot = poligono.verticesRotacionados;
                KeyDownDelay = 0.0f;
            }

            if (KeyDownDelay >= KeyDownDelayTime)
            {
                if (IsKeyDown(KEY_UP))
                {
                    VLN = VLN + 1;
                    cortar = VLN;
                    poligono.setVertices(collectedPoints);
                    poligono.cortarPoligonal(poligono.verticesRotacionados, cortar);
                    Rot = poligono.verticesRotacionados;
                    KeyDownDelay = 0.0f;
                }
                if (IsKeyDown(KEY_DOWN))
                {
                    VLN = VLN + -1;
                    cortar = VLN;
                    poligono.setVertices(collectedPoints);
                    poligono.cortarPoligonal(poligono.verticesRotacionados, cortar);
                    Rot = poligono.verticesRotacionados;
                    KeyDownDelay = 0.0f;
                }
            }

            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
            {
                if (IsKeyPressed(KEY_LEFT))
                    graus += 15;
                if (IsKeyPressed(KEY_RIGHT))
                    graus -= 15;

                TraceLog(LOG_INFO, "Angulo %.2f", graus);

                poligono.setVertices(collectedPoints);
                poligono.translacaoCG(poligono.vertices); 
                poligono.rotacao(graus);                  
                poligono.cortarPoligonal(poligono.verticesRotacionados, cortar);
                Rot = poligono.verticesRotacionados; 
                
                Point centroide = poligono.centroide();

                reforco.translacaoCG(reforco.Armaduras, centroide);
                reforco.RotacionarArmadura(graus);

                TraceLog(LOG_INFO, "Vertices Armaduras (Originais)");
                for (const auto &point : reforco.Armaduras)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }

                TraceLog(LOG_INFO, "Barras Transladadas e Rotacionadas");
                for (const auto &point : reforco.barrasRotacionadas)
                {
                    TraceLog(LOG_INFO, "x = %.2f, y = %.2f", point.x, point.y);
                }
            }

            int numPoints = Rot.size();

            for (int i = 0; i <= numPoints; i++)
            {
                if (Rot[i].x > valorMaior)
                {
                    valorMaior = Rot[i].x;
                }

                if (Rot[i].x < valorMenor)
                {
                    valorMenor = Rot[i].x;
                }
            }

            x_values[0] = valorMenor - (valorMaior - valorMenor) * 0.1;
            x_values[1] = valorMaior + (valorMaior - valorMenor) * 0.1;

            // Ajuste o código de desenho no gráfico
            if (numPoints >= 3)
            {
                // Fechar os vetores adicionando o primeiro ponto ao final
                std::vector<Point> rotacionadosFechados = Rot;
                poligono.fecharPoligono(rotacionadosFechados);
                std::vector<Point> collectedPointsFechados = collectedPoints;
                poligono.fecharPoligono(collectedPointsFechados);
                std::vector<Point> resultadoCorteFechado = poligono.resultadoCorte;
                poligono.fecharPoligono(resultadoCorteFechado);
                std::vector<Point> AreaSuperiorFechado = poligono.areaSuperior;
                poligono.fecharPoligono(AreaSuperiorFechado);
                std::vector<Point> AreaInferiorFechado = poligono.areaInferior;
                poligono.fecharPoligono(AreaInferiorFechado);
                std::vector<Point> ArmadurasFechadas = reforco.Armaduras;
                poligono.fecharPoligono(ArmadurasFechadas);
                std::vector<Point> ArmaduraRotFechada = reforco.barrasRotacionadas;
                poligono.fecharPoligono(ArmaduraRotFechada);

                // Converter para arrays de float para os gráficos
                float x_data[collectedPointsFechados.size()];
                float y_data[collectedPointsFechados.size()];
                float x_corte[resultadoCorteFechado.size()];
                float y_corte[resultadoCorteFechado.size()];
                float x_superior[AreaSuperiorFechado.size()];
                float y_superior[AreaSuperiorFechado.size()];
                float x_inferior[AreaInferiorFechado.size()];
                float y_inferior[AreaInferiorFechado.size()];
                float xRot[rotacionadosFechados.size()];
                float yRot[rotacionadosFechados.size()];
                float xArmaduraRotacionada[ArmaduraRotFechada.size()];
                float yArmaduraRotacionada[ArmaduraRotFechada.size()];

                for (size_t i = 0; i < ArmaduraRotFechada.size(); i++)
                {
                    xArmaduraRotacionada[i] = ArmaduraRotFechada[i].x;
                    yArmaduraRotacionada[i] = ArmaduraRotFechada[i].y;
                }
                for (size_t i = 0; i < rotacionadosFechados.size(); i++)
                {
                    xRot[i] = rotacionadosFechados[i].x;
                    yRot[i] = rotacionadosFechados[i].y;
                }

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
                    ImPlot::PlotScatter("Vértices Rotacionados", xRot, yRot, (rotacionadosFechados.size()));                     
                    ImPlot::PlotScatter("Armadura Rotacionada", xArmaduraRotacionada, yArmaduraRotacionada, ArmaduraRotFechada.size());
                    ImPlot::PlotLine("Polígono", x_data, y_data, collectedPointsFechados.size());
                    ImPlot::PlotLine("Polígono cortado", x_corte, y_corte, resultadoCorteFechado.size());
                    ImPlot::PlotLine("Polígono superior", x_superior, y_superior, AreaSuperiorFechado.size());
                    ImPlot::PlotLine("Polígono inferior", x_inferior, y_inferior, AreaInferiorFechado.size());
                    ImPlot::PlotLine("Linha de corte", x_values, y_values, 2);
                    ImPlot::PlotLine("Polígono Rotacionado", xRot, yRot, rotacionadosFechados.size());
                    ImPlot::EndPlot();
               
                }
            }
            else
            {
                ImGui::Text("Insira pelo menos 3 vértices para formar um polígono.");
            } 
 
            ImGui::End(); // Finaliza a janela do gráfico
        } 
        */
        rlImGuiEnd();
        EndDrawing();
    }

    // Finaliza o contexto
    //ImPlot::DestroyContext();
    rlImGuiShutdown();
}

int main()
{
    std::cout << "Iniciando a interface..." << std::endl;
    IniciarInterface();
    std::cout << "Entrando no loop do programa..." << std::endl;
    loopPrograma();
    return 0;
}