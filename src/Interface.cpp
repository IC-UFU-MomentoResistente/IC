#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "AppView.h"
#include "interface.h"

Polygon polygon;
ImFont *font = nullptr;

void Interface::InitInterface() 
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
    ImFontConfig fontConfig;
    static const ImWchar customRange[] =
        {
            0x0020, 0x00FF, // Faixa básica (ASCII estendido)
            0x0370, 0x03FF, // Faixa de grego
            0};
    font = ImGui::GetIO().Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);
            
    
    rlImGuiEndInitImGui();
    ImPlot::CreateContext();
}

void Interface::InterfaceLoop() 
{
  

}

void Interface::AutorsWindow() {
    ImGui::SetNextWindowPos(ImVec2(140, 20));
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
    ImGui::BulletText("Prof Dr Eduardo Vicente Wolf Trentini");
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

void Interface::ShowSecondaryMenuBar() 
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
        CrossSectionData();
        



        ImGui::EndMenuBar();
        
    }
                    
    ImGui::End();
}

void Interface::CrossSectionData() 
{
    
    if (ImGui::BeginMenu("Seção Transversal")) // Primeira versão, não é a final - precisa incrementar vértices temporarios - não adicionar vertices negativos e tal   
    {
        static double coordXPolygon, coordYPolygon;
                
        ImGui::SetNextWindowPos(ImVec2(4,47));
        ImGui::SetNextWindowSize(ImVec2(420,270));
        ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove);

        // Input para número de pontos
        
        ImGui::SetNextItemWidth(80);

        ImGui::Spacing(); // Adiciona um espaçamento antes da tabela

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

        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::InterfaceMaterials()
{

    ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always);    // Ajuste os valores conforme necessário
    ImGui::SetNextWindowPos(ImVec2(123, 47)); // Posição inicial
    ImGui::Begin("Entrada de Dados de Materiais", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    int opcao; 

    if (ImGui::BeginTabBar("Tabela de Entrada de Dados de Materiais"))
    {

    if (ImGui::BeginMenu("Materiais"))
    {
        if (ImGui::BeginTabItem("Concreto"))
        {
            ImGui::Text("Tipo de Diagrama:");
            ImGui::RadioButton("Parábola-Retangulo (NBR 6118:2023)", &opcao, 0);
            ImGui::RadioButton("Parábola-Retangulo (NBR 6118:2014)", &opcao, 1);

            if(opcao == 0)
            {
                ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 160);
                ImGui::Text("Parâmetros do Concreto");
            }

            if(opcao == 1) 
            {
                ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 160);
                ImGui::Text("Parâmetros do Concreto");
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Armadura Passiva"))
        {
            ImGui::EndTabItem();
        }

        ImGui::EndMenuBar();
    }
    
    
    ImGui::EndTabBar();
    }
}
