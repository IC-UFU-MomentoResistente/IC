#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "AppView.h"
#include "interface.h"


void Interface::initInterface() 
{
   
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int screenWidth = 1280;
    int screenHeight = 960;
    
    InitWindow(screenWidth, screenHeight, "SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO");

    if (!IsWindowReady()) // Verifique se a janela foi criada com sucesso
    {
        std::cerr << "Erro ao criar a janela!" << std::endl;
        return; // Saia da função se a janela não foi criada
    }
    
    
    rlImGuiBeginInitImGui();
    ImGui::StyleColorsDark();
    
    /* ImFontConfig fontConfig;
    static const ImWchar customRange[] =
        {
            0x0020, 0x00FF, // Faixa básica (ASCII estendido)
            0x0370, 0x03FF, // Faixa de grego
            0};
    font = ImGui::GetIO().Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);
            */
    
    rlImGuiEndInitImGui();
    ImPlot::CreateContext();
}

void Interface::interfaceLoop() 
{
  
}



void Interface::showPrimaryMenuBar() 
{
    if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Arquivo"))
            {
                ImGui::MenuItem("Salvar");
                ImGui::MenuItem("Carregar");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Resultado"))
            {
                ImGui::MenuItem("Gerar relatório");
                ImGui::MenuItem("Visualizar Gráficos");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Autores"))
            {
                autorsWindow();
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
}

void Interface::autorsWindow() {
   
    ImGui::SetNextWindowPos(ImVec2(145, 20));
    ImGui::SetNextWindowSize(ImVec2(600, 250));
    ImGui::Begin("Autores", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration);
    ImGui::Text("SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Desenvolvido por:");
    ImGui::BulletText("Arthur C. Pena - arthur.cunha.pena@ufu.br");
    ImGui::BulletText("Gabriel A. P. Lunarti - gabriel.lunarti@ufu.br");
    ImGui::Spacing();
    ImGui::Text("Orientador:");
    ImGui::BulletText("Prof Dr Eduardo Vicente Wolf Trentini - etrentini@ufu.br ");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(25); // Centraliza o botão
    if (ImGui::Button("Fechar", ImVec2(100, 30)))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();
}

void Interface::showSecondaryMenuBar(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, AppView &view)	 
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 window_size = ImVec2(io.DisplaySize.x, ImGui::GetFrameHeight());

    // Configura a posição e o tamanho da nova janela
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);

    // Cria uma janela sem decorações com uma barra de menu
    ImGui::Begin("##SecondaryMenuBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar);
    
    if(ImGui::BeginMenuBar()) 
    {
        crossSectionData(polygon, reinforcement);
        interfaceMaterials(concrete, view);
        reinforcementInterface(reinforcement);
        effortSectionInterface();

        ImGui::EndMenuBar();
    }
    
    ImGui::End(); // Finaliza a janela
}

void Interface::crossSectionData(Polygon &polygon, Reinforcement &reinforcement) 
{
    if (ImGui::BeginMenu("Seção Transversal")) // Primeira versão, não é a final - precisa incrementar vértices temporarios - não adicionar vertices negativos e tal   
    {
        static double coordXPolygon, coordYPolygon;
                
        ImGui::SetNextWindowPos(ImVec2(4,35));
        ImGui::SetNextWindowSize(ImVec2(420,270));
        ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove);

        // Input para número de pontos
        ImGui::SetNextItemWidth(80);
        ImGui::Spacing(); // Adiciona um espaçamento antes da tabela
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

        ImGui::End(); // Finaliza a janela
        ImGui::EndMenu();
    }
}

void Interface::interfaceMaterials(ConcreteProperties &concrete, AppView &view)
{

    int option = 0;
    float fck_variable;
    float yc_variable;
    float beta_variable;
    float fyk_variable; 
    float gama_s_variable;
    float es_variable;

    if(ImGui::BeginMenu("Materiais")) 
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always);    // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(138, 35)); // Posição inicial
        ImGui::Begin("Entrada de Dados de Materiais", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImVec2 plotSize = ImGui::GetContentRegionAvail();
        ImGui::Text("εc");

        static int constitutiveModel = 0;
        static double collectedFck, collectedGammaC, stress;

        ImGui::RadioButton("ABNT NBR 6118:2014", &constitutiveModel, 0);
        ImGui::RadioButton("ABNT NBR 6118:2023", &constitutiveModel, 1);

        if (constitutiveModel == 0)
        {
            StressStrainModelType model61182014 = StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2014;
            ImGui::InputDouble("fck (MPa):", &collectedFck);
            ImGui::InputDouble("gammaC: ", &collectedGammaC);

            if (collectedFck < 0 || collectedGammaC < 0)
            {
                collectedFck = 0;
                collectedGammaC = 1;
            }

            if (ImGui::Button("Adicionar"))
            {
                concrete.setParameters(model61182014, collectedFck, collectedGammaC);

                concrete.setCurveStressStrain();
            }

            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            // inicialização do gráfico com os eixos
            if (ImPlot::BeginPlot("Concreto", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
            {
                ImPlot::SetupAxesLimits(0, (concrete.getStrainConcreteRupture() * 1.1), 0, concrete.getFcd(), ImGuiCond_Always);
                view.renderReinforcement(concrete.getCurveStressStrain(), "TensaoxDef");
            }

            ImPlot::EndPlot();
        }

        if (constitutiveModel == 1)
        {
            StressStrainModelType model61182023 = StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2023;
            ImGui::InputDouble("fck (MPa):", &collectedFck);
            ImGui::InputDouble("gammaC: ", &collectedGammaC);

            if (collectedFck < 0 || collectedGammaC < 0)
            {
                collectedFck = 0;
                collectedGammaC = 1;
            }

            if (ImGui::Button("Adicionar"))
            {
                concrete.setParameters(model61182023, collectedFck, collectedGammaC);

                concrete.setCurveStressStrain();
            }

            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            // inicialização do gráfico com os eixos
            if (ImPlot::BeginPlot("Concreto", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
            {
                ImPlot::SetupAxesLimits(0, (concrete.getStrainConcreteRupture() * 1.1), 0, concrete.getFcd(), ImGuiCond_Always);
                view.renderReinforcement(concrete.getCurveStressStrain(), "TensaoxDef");
            }

            ImPlot::EndPlot();
        }

        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::reinforcementInterface(Reinforcement &reinforcement)
{
    if (ImGui::BeginMenu("Armadura"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(200, 35));                     // Posição inicial
        static int barMode, numBar = 0;
        static double coordXBar, coordYBar, diameterBar, coordXiBar, coordXfBar, coordYiBar, coordYfBar;

        ImGui::Begin("Entrada de dados: Armadura Passiva", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
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
        ImGui::EndMenu();
    }
}

void Interface::effortSectionInterface()
{
    if (ImGui::BeginMenu("Esforços"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(200, 35));                     // Posição inicial
        static double N, Mx, My;

        ImGui::Begin("Entrada de Dados: Esforços", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::InputDouble("N (kN)", &N);
        ImGui::InputDouble("Mx (kN.m)", &Mx);
        ImGui::InputDouble("My (kN.m)", &My);

        if (ImGui::Button("Calcular"))
        {
            // Calcular o momento resistente
        }

        ImGui::End();
        ImGui::EndMenu();
    }   
}

void Interface::crossSectionPlotInterface(AppView &view, Polygon &polygon, Reinforcement &reinforcement) 
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