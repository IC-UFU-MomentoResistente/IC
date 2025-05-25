#include "polygonJSONRepository.h"
#include "createPolygonUseCase.h"
#include "getPolygonUseCase.h"
#include "savePolygonUseCase.h"
#include "polygonController.h"
#include "point.h"

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include <iostream>
#include <string>
#include <vector>

// Variáveis globais
static std::vector<Point> atualVerticesUI;
static char namePolygonInput[128] = "";
static Polygon loadedPolygon;
static bool isLoadedPolygon = false;
static std::vector<std::string> namesPolygons;
static int itemSelected = -1;
static char namePolygonToLoad[128] = "";

// função para desenhar a poligonal com Raylib
void drawPolygon(const Polygon& polygon, Color color)
{
    if (polygon.vertices.size() < 3) return; 

    // desenha as linhas da poligonal
    for (size_t i = 0; i < polygon.vertices.size(); ++i)
    {
        Point p1 = polygon.vertices[i];
        Point p2 = polygon.vertices[(i + 1) % polygon.vertices.size()];
        DrawLineV({(float)p1.x, (float)p1.y}, {(float)p2.x, (float)p2.y}, color);
    }

    // desenha os vértices
    for (const auto& v : polygon.vertices)
    {
        DrawCircleV({(float)v.x, (float)v.y}, 3.0f, RED);
    }
}

int main()
{
    // Injeção de denpendência
    PolygonJSONRepository repo("./data_polygon");

    CreatePolygonUseCase createUC(repo);
    GetPolygonUseCase getUC(repo);
    SavePolygonUseCase saveUC(repo);

    PolygonController controller(createUC, getUC, saveUC);
    // ===========================================================

    // Configuração Raylib e ImGui
    int screenWidth = 1280;
    int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Calculadora Momento Resistente");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    // ===========================================================

    // Carregar os nomes dos polígonos salvos
    try
    {
        namesPolygons = controller.getSavedPolygonNames();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Erro ao carregar nomes das seções: " << e.what() << '\n';
    }
    // ===========================================================

    // Loop principal
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Lógica com ImGui
        rlImGuiBegin();

        ImGui::Begin("Editor de Seção Poligonal");

        ImGui::InputText("Nome da seção", namePolygonInput, IM_ARRAYSIZE(namePolygonInput));

        ImGui::Text("Vértices (clique na área de desenho ou adicione manualmente):");
        // Lista de vértices e campos para adicionar/remover (simplificada)
        for (size_t i = 0; i < atualVerticesUI.size(); ++i)
        {
            ImGui::Text("V%zu: (%.2f, %.2f)", i + 1, atualVerticesUI[i].x, atualVerticesUI[i].y);
        }

        if (ImGui::Button("Limpar Vértices"))
        {
            atualVerticesUI.clear();
            loadedPolygon.vertices.clear();
            isLoadedPolygon = false;
        }

        if (ImGui::Button("Criar e Salvar Seção"))
        {
            if (strlen(namePolygonInput) > 0 && atualVerticesUI.size() >= 3)
            {
                PolygonDto data;
                data.name = namePolygonInput;
                data.vertices = atualVerticesUI;

                try
                {
                    loadedPolygon = controller.createNewPolygon(data);
                    isLoadedPolygon = true;
                    std::cout << "Secao: '" << loadedPolygon.name << "' criada e salva com ID: " << loadedPolygon.id << '\n';
                    namesPolygons = controller.getSavedPolygonNames();
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Erro ao criar a secao: " << e.what() << '\n';
                }
            }
            else
            {
                std::cerr << "Erro: Nome da seção inválido ou número insuficiente de vértices." << '\n';
            }
        }

        ImGui::Separator();
        ImGui::Text("Carregar Secao Salva:");

        // Dropdown para carregar seções
        std::vector<const char*> namesForDropdown;
        for (const auto& name : namesPolygons)
        {
            namesForDropdown.push_back(name.c_str());
        }

        if (namesForDropdown.empty())
        {
            ImGui::Text("Nenhuma seção salva encontrada.");
        }
        else
        {
            if (ImGui::Combo("Selecione para Carregar", &itemSelected, namesForDropdown.data(), namesForDropdown.size()))
            {
                if (itemSelected >= 0 && itemSelected < namesPolygons.size())
                {
                    strncpy(namePolygonToLoad, namesPolygons[itemSelected].c_str(), sizeof(namePolygonToLoad) -1);
                    namePolygonToLoad[sizeof(namePolygonToLoad)-1] = '\0';
                }
            }
        }


        // Input para nome da seção a carregar (alternativa ao dropdown)
        // ImGui::InputText("Nome para Carregar", nomeSecaoParaCarregar, IM_ARRAYSIZE(nomeSecaoParaCarregar));
        if (ImGui::Button("Carregar Seção")) {
             if (strlen(namePolygonToLoad) > 0) {
                try {
                    auto optSecao = controller.loadPolygonByName(namePolygonToLoad);
                    if (optSecao) {
                        loadedPolygon = *optSecao;
                        atualVerticesUI = loadedPolygon.vertices; // Atualiza UI com vértices carregados
                        strncpy(namePolygonInput, loadedPolygon.name.c_str(), sizeof(namePolygonInput)-1); // Atualiza nome na UI
                        namePolygonInput[sizeof(namePolygonInput)-1] = '\0';
                        isLoadedPolygon = true;
                        std::cout << "Seção '" << loadedPolygon.name << "' carregada." << std::endl;
                    } else {
                        std::cout << "Seção '" << namePolygonToLoad << "' não encontrada." << std::endl;
                        isLoadedPolygon = false;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Erro ao carregar seção: " << e.what() << std::endl;
                }
            }
        }

        ImGui::End();

        // Lógica para adicionar vértices com clique do mouse (fora da janela ImGui)
        // Define uma área de desenho, por exemplo, à direita da UI do ImGui
        float uiWidth = 300.0f; // Largura estimada da UI ImGui
        Rectangle drawArea = { uiWidth, 0, (float)screenWidth - uiWidth, (float)screenHeight };
        
        DrawRectangleLinesEx(drawArea, 1, LIGHTGRAY); // Borda da área de desenho

        if (CheckCollisionPointRec(GetMousePosition(), drawArea) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive())
            {
                // Mouse está sobre uma janela ImGui, não adicione vértice
            } 
            else 
            {
                Point newPoint = { GetMousePosition().x, GetMousePosition().y };
                atualVerticesUI.push_back(newPoint);
                if (isLoadedPolygon) 
                { // Se uma seção está carregada, adiciona à ela também
                    loadedPolygon.addVertice(newPoint);
                }
            }
        }

        // Desenha a poligonal que está sendo editada ou a carregada
        if (!atualVerticesUI.empty())
        {
            Polygon tempPoligonalVisual(0, "temp", atualVerticesUI);
            drawPolygon(tempPoligonalVisual, BLUE);
        } 
        else if (isLoadedPolygon) 
        {
            drawPolygon(loadedPolygon, DARKGREEN);
        }

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}