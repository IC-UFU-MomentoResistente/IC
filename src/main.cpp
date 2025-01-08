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


std::vector<Point> collectedPoints = {{-10, -20}, {10, -20}, {10, 20}, {-10, 20}}; 

Poligono poligono;
Poligono poligonoComprimido;
Poligono poligonoAreaParabola;
Poligono poligonoAreaRetangulo;

std::vector<Point> Rot = collectedPoints;
std::vector<Point> pontosOriginais = collectedPoints;

float graus = 0;
float yMaxSecao = 0;
float yMinSecao = 0;
float yMinArmadura = 0;

Point centroideInicial;

void IniciarInterface()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int screenWidth = 1280;
    int screenHeight = 960;
        
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
float VLN = 0;
float cortar = 0;
float diametroBarras = 0;
float barrasPosXi = 0;
float barrasPosYi = 0;
float barrasPosXf = 0;
float barrasPosYf = 0;
float fck = 0;
float gama_c = 0;
float eps1 = 0;
float eps2 = 0;
float x_d = 0;
float d = 0;
bool janelaGrafico = true;
bool janelaPoligono = true;
bool tabelaArmadura = true;
bool janelaConcreto = true;
bool janelaPoligonoComprimido = true;
bool janelaAcoPassivo = true; 

float fyk_variavel = 500.0f;
float gama_s_variavel = 1.15f; 
float Ep2_variavel = -10.907f;
float Ep1_variavel = 1.189f;
float Es_variavel = 210.0f;

void loopPrograma()
{
    reforco.AdicionarBarra(-7.f, -17.f, 1.f);
    reforco.AdicionarBarra(+7.f, -17.f, 1.f);
    while (!WindowShouldClose())
    {
        DrawFPS(20, 20);
        KeyDownDelay = KeyDownDelay + GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        if (janelaGrafico)
        {
            ImGui::Begin("Gráfico da Seção Transversal", &janelaGrafico); // Título da janela
            
            int ARR_SIZE = 2;
            float x_values[ARR_SIZE]; // teste
            float y_values[ARR_SIZE];

            float valorMenor = FLT_MAX;  // Número de ponto flutuante representável máximo.
            float valorMaior = -FLT_MAX; // Número de ponto flutuante representável máximo.

            for (int i = 0; i < ARR_SIZE; i++)
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

            for (int i = 0; i < numPoints; i++)
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
                //float xArmadura[ArmadurasFechadas.size()]; 
                //float yArmadura[ArmadurasFechadas.size()];
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
                if (ImPlot::BeginPlot("Gráfico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
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

       /* if (janelaConcreto)
        {
            ImGui::Begin ("Entradas de dados: Parâmetros Concreto", &janelaConcreto);
            ImGui::Text("Insira os valores de fck e gama_c");
            
            ImGui::InputFloat("fck", &fck);
            ImGui::InputFloat("gama_c", &gama_c);
            ImGui::InputFloat("eps1", &eps1);
            ImGui::InputFloat("eps2", &eps2);

            poligono.MaxMin(yMaxSecao, yMinSecao);
            centroideInicial = poligono.centroide();
            reforco.Min(yMinArmadura);

            if (ImGui::Button("Calcular parâmetros"))
            {
                Concreto concreto (fck, gama_c, eps1, eps2, x_d, yMaxSecao, yMinSecao, yMinArmadura, centroideInicial.y);
                Concreto::ParametrosConcreto parametrosConcreto = concreto.getParametros();
                Concreto::AlturasConcreto alturasConcreto = concreto.getAlturas();
                float corte_LN = alturasConcreto.altura_LN;
                poligono.cortarPoligonal(poligono.verticesRotacionados, corte_LN);
                poligonoComprimido.setVertices(poligono.areaSuperior);
                float corte_def_2 = alturasConcreto.altura_deformacao_2;
                poligonoComprimido.cortarPoligonal(poligonoComprimido.areaSuperior, corte_def_2);
                poligonoAreaParabola.setVertices(poligonoComprimido.areaInferior);
                poligonoAreaRetangulo.setVertices(poligonoComprimido.areaSuperior);

                TraceLog(LOG_INFO, "Parâmetros do Concreto Calculados");
                TraceLog(LOG_INFO, "Fator multiplicativo: %.3f", parametrosConcreto.fatorMultTensaoCompConcreto);
                TraceLog(LOG_INFO, "Ep ultimo: %.5f", parametrosConcreto.epsilonConcretoUltimo);
                TraceLog(LOG_INFO, "Ep2: %.5f", parametrosConcreto.epsilonConcreto2);
                TraceLog(LOG_INFO, "Expoente: %.2f", parametrosConcreto.expoenteTensaoConcreto);
                TraceLog(LOG_INFO, "fcd: %.2f", parametrosConcreto.fcd);
                TraceLog(LOG_INFO, "altura 2/1000: %.2f", alturasConcreto.altura_deformacao_2);
                TraceLog(LOG_INFO, "altura ultima: %.2f", alturasConcreto.altura_deformacao_ultima);
                TraceLog(LOG_INFO, "altura LN: %.2f", alturasConcreto.altura_LN);
                TraceLog(LOG_INFO, "altura d: %.2f", alturasConcreto.d);
                TraceLog(LOG_INFO, "altura h: %.2f", concreto.h_secao);
                TraceLog(LOG_INFO, "yMaxSecao: %.2f", yMaxSecao);
                TraceLog(LOG_INFO, "yMinSecao: %.2f", yMinSecao);
                TraceLog(LOG_INFO, "yMinArmadura: %.2f", yMinArmadura);
            }

            ImGui::End();
        } */


        float fyd_variavel; 
        float epsilon_yd_variavel; 

        float epi_variavel;
        
        if (janelaAcoPassivo)
        {
            ImGui::Begin("Parâmetros do Aço Passivo", &janelaAcoPassivo);
            ImGui::InputFloat("FYK:", &fyk_variavel);
            ImGui::InputFloat("Gama_S:", &gama_s_variavel);
            ImGui::InputFloat("Es:", &Es_variavel);
            ImGui::InputFloat("EP2:", &Ep2_variavel);
            ImGui::InputFloat("Ep1:", &Ep1_variavel);

            if(ImGui::Button("Pressione aqui para o cálculo de parâmetros FYD e Epyd"))
            {
                    reforco.calculaParametros(fyk_variavel, gama_s_variavel, Es_variavel);
                    fyd_variavel = reforco.fyd;
                    epsilon_yd_variavel = reforco.epsilon_yd;
                    TraceLog(LOG_INFO, "fyd = %f", fyd_variavel);
                    TraceLog(LOG_INFO, "epsilon_yd = %f", epsilon_yd_variavel);                                   
                    reforco.calculaNormal_Momento(Ep2_variavel, Ep1_variavel);
            }

            ImGui::End();
        
        }


        if (janelaPoligonoComprimido)
        {
            ImGui::Begin ("Poligono comprimido", &janelaPoligonoComprimido);

            std::vector<Point> poligonoComprimidoFechado = poligonoComprimido.vertices;
            poligonoComprimido.fecharPoligono(poligonoComprimidoFechado);
            std::vector<Point> poligonoAreaParabolaFechado = poligonoAreaParabola.vertices;
            poligonoAreaParabola.fecharPoligono(poligonoAreaParabolaFechado);
            std::vector<Point> poligonoAreaRetanguloFechado = poligonoAreaRetangulo.vertices;
            poligonoAreaRetangulo.fecharPoligono(poligonoAreaRetanguloFechado);

            float x_poligonoComprimido[poligonoComprimidoFechado.size()];
            float y_poligonoComprimido[poligonoComprimidoFechado.size()];;
            float x_poligonoAreaParabola[poligonoAreaParabolaFechado.size()];
            float y_poligonoAreaParabola[poligonoAreaParabolaFechado.size()];
            float x_poligonoAreaRetangulo[poligonoAreaRetanguloFechado.size()];
            float y_poligonoAreaRetangulo[poligonoAreaRetanguloFechado.size()];

            for(size_t i = 0; i < poligonoComprimidoFechado.size(); i++)
            {
                x_poligonoComprimido[i] = poligonoComprimidoFechado[i].x;
                y_poligonoComprimido[i] = poligonoComprimidoFechado[i].y;
            }

            for (size_t i = 0; i < poligonoAreaParabolaFechado.size(); i++)
            {
                x_poligonoAreaParabola[i] = poligonoAreaParabolaFechado[i].x;
                y_poligonoAreaParabola[i] = poligonoAreaParabolaFechado[i].y;
            }

            for (size_t i = 0; i < poligonoAreaRetanguloFechado.size(); i++)
            {
                x_poligonoAreaRetangulo[i] = poligonoAreaRetanguloFechado[i].x;
                y_poligonoAreaRetangulo[i] = poligonoAreaRetanguloFechado[i].y;
            }

            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            // Plota os pontos e desenha o polígono
            if (ImPlot::BeginPlot("Gráfico", ImVec2(plotSize.x, plotSize.y)))
            {
                ImPlot::PlotScatter("Vertices comprimidos", x_poligonoComprimido, y_poligonoComprimido, poligonoComprimidoFechado.size());
                ImPlot::PlotScatter("Vertices parabola", x_poligonoAreaParabola, y_poligonoAreaParabola, poligonoAreaParabolaFechado.size());
                ImPlot::PlotScatter("Vertices retangulo", x_poligonoAreaRetangulo, y_poligonoAreaRetangulo, poligonoAreaRetanguloFechado.size());

                ImPlot::PlotLine("Poligono Comprimido", x_poligonoComprimido, y_poligonoComprimido, poligonoComprimidoFechado.size());
                ImPlot::PlotLine("Poligono parabola", x_poligonoAreaParabola, y_poligonoAreaParabola, poligonoAreaParabolaFechado.size());
                ImPlot::PlotLine("Poligono retangulo", x_poligonoAreaRetangulo, y_poligonoAreaRetangulo, poligonoAreaRetanguloFechado.size());

                ImPlot::EndPlot();
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

        rlImGuiEnd();
        EndDrawing();
    }

    // Finaliza o contexto
    ImPlot::DestroyContext();
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