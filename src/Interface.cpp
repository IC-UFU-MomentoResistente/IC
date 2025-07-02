#define IMGUI_DEFINE_MATH_OPERATORS
#include "Interface.h"
#include "segoeuisl_data.h"

#include <iostream>
#include <functional>
#include <algorithm>
#include <imgui_internal.h>

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

    ImFontConfig fontConfig;
    static const ImWchar customRange[] = {
        0x0020, 0x00FF, // ASCII estendido
        0x0370, 0x03FF, // Grego
        0x2030, 0x2030, // Símbolo de por mil (‰)
        0};

    ImGuiIO &io = ImGui::GetIO();
    // ImFont *customFont = io.Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);

    ImFont *customFont = io.Fonts->AddFontFromMemoryTTF(
        (void *)segoeuisl_ttf,
        segoeuisl_ttf_len,
        18.0f,
        &fontConfig,
        customRange);

    if (customFont)
    {
        io.FontDefault = customFont;
    }
    else
    {
        std::cerr << "Erro ao carregar a fonte personalizada!" << std::endl;
    }

    rlImGuiEndInitImGui();
    ImPlot::CreateContext();
}

void Interface::interfaceLoop()
{
}

void Interface::showPrimaryMenuBar(Section &section)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Arquivo"))
        {
            if (ImGui::MenuItem("Novo"))
            {
                clearSection(section);
            }

            if (ImGui::MenuItem("Salvar"))
            {
                if (section.originalPolygon.getPolygonVertices().empty() && section.originalReinforcement.getReinforcement().empty())
                {
                    showEmptySectionSaveError = true;
                }
                else
                {
                    IGFD::FileDialogConfig configSave;
                    configSave.path = ".";
                    configSave.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
                    ImGuiFileDialog::Instance()->OpenDialog("SaveFileDialog", "Salvar Projeto", ".json", configSave);
                }    
            }

            if (ImGui::MenuItem("Carregar"))
            {
                IGFD::FileDialogConfig configLoad;
                configLoad.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("LoadFileDialog", "Carregar Projeto", ".json", configLoad);
                shouldAutoFit = true;
                shouldAutoFitEnv = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Autores"))
        {
            autorsWindow();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Obtém o tamanho da área de exibição do ImGui
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Define um tamanho desejado para o diálogo, como 40% da largura/altura da tela
    ImVec2 dialogCurrentSize = displaySize * 0.4f;

    // Define os tamanhos mínimo e máximo para o diálogo
    static ImVec2 dialogMinSize = ImVec2(400, 300);
    static ImVec2 dialogMaxSize = ImVec2(displaySize.x, displaySize.y);

    // Garante que o tamanho atual do diálogo esteja dentro dos limites min/max
    dialogCurrentSize.x = ImMax(dialogCurrentSize.x, dialogMinSize.x);
    dialogCurrentSize.y = ImMax(dialogCurrentSize.y, dialogMinSize.y);
    dialogCurrentSize.x = ImMin(dialogCurrentSize.x, dialogMaxSize.x);
    dialogCurrentSize.y = ImMin(dialogCurrentSize.y, dialogMaxSize.y);

    // Calcula a posição para centralizar a janela
    // Subtrai metade do tamanho do diálogo da metade do tamanho da tela
    ImVec2 centeredPos;
    centeredPos.x = (displaySize.x - dialogCurrentSize.x) * 0.5f;
    centeredPos.y = (displaySize.y - dialogCurrentSize.y) * 0.5f;

    // --- Fim das modificações ---

    // Aplica a posição calculada antes de exibir o File Dialog de Salvar
    ImGui::SetNextWindowPos(centeredPos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(dialogCurrentSize, ImGuiCond_Appearing);

    if (ImGuiFileDialog::Instance()->Display("SaveFileDialog"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            saveSectionData(section, filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Aplica a posição calculada antes de exibir o File Dialog de Carregar
    ImGui::SetNextWindowPos(centeredPos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(dialogCurrentSize, ImGuiCond_Appearing);

    if (ImGuiFileDialog::Instance()->Display("LoadFileDialog"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            loadSectionData(section, filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (showCorrupedFileError)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::OpenPopup("Erro ao Carregar Projeto");
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro ao Carregar Projeto", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("O arquivo selecionado está corrompido ou não é um arquivo de projeto válido.");
            ImGui::Text("Por favor, selecione um arquivo de projeto válido ou crie um novo.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showCorrupedFileError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (showEmptySectionSaveError)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::OpenPopup("Erro ao Salvar Projeto");
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro ao Salvar Projeto", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Não é possível salvar um projeto sem uma seção transversal definida.");
            ImGui::Text("Por favor, defina a geometria da seção (Poligonal, Retangular, T ou Circular)");
            ImGui::Text("e/ou adicione a armadura antes de salvar.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showEmptySectionSaveError = false; // Reseta a flag para fechar o popup
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void Interface::autorsWindow()
{
    ImGui::SeparatorText("Software de cálculo do momento resistente em seções de concreto armado");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Desenvolvido por:");
    ImGui::BulletText("Arthur C. Pena - arthur.cunha.pena@ufu.br");
    ImGui::BulletText("Gabriel A. P. Lunarti - gabriel.lunarti@ufu.br");
    ImGui::Spacing();
    ImGui::Text("Orientador:");
    ImGui::BulletText("Prof. Dr. Eduardo Vicente Wolf Trentini - etrentini@ufu.br ");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(25); // Centraliza o botão
    if (ImGui::Button("Fechar", ImVec2(100, 30)))
    {
        ImGui::CloseCurrentPopup();
    }
}

void Interface::showSecondaryMenuBar(Section &section)
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

    if (ImGui::BeginMenuBar())
    {
        crossSectionData(section);
        interfaceMaterials(section);
        reinforcementInterface(section);
        effortSectionInterface(section);

        ImGui::EndMenuBar();
    }

    ImGui::End(); // Finaliza a janela
}

void Interface::crossSectionData(Section &section)
{
    if (ImGui::BeginMenu("Seção Transversal"))
    {
        ImGui::SetNextWindowPos(ImVec2(3, 47));
        ImGui::SetNextWindowSize(ImVec2(420, 400)); 

        ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        
        if (ImGui::BeginTabBar("Tabela de Entrada de Dados da Seção Transversal"))
        {
            if (ImGui::BeginTabItem("Poligonal"))
            {
                inputSectionPolygonal(section);
                clearInputSection(section);
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Seção Retangular"))
            {
                inputSectionRectangle(section);
                clearInputSection(section);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Seção T"))
            {
                inputSectionT(section);
                clearInputSection(section);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Seção Circular"))
            {
                inputSectionCircular(section);
                clearInputSection(section);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Debug"))
            {
                inputSectionDebug(section);
                clearInputSection(section);
                ImGui::EndTabItem();
            }
            
            showGeometricParameters(section);

            ImGui::EndTabBar();
        }
        
        ImGui::End();     // Fim da janela
        ImGui::EndMenu(); // Fim do menu
    }
}

void Interface::inputSectionRectangle(Section &section)
{
    static float rectBase = 20.0f; 
    static float rectHeight = 40.0f;
    
    ImGui::SeparatorText("Dados da Seção Retangular:");
    ImGui::InputFloat("Base (cm)", &rectBase);
    ImGui::InputFloat("Altura (cm)", &rectHeight);

    if (ImGui::Button("Gerar Retângulo"))
    {
        section.originalPolygon.clearPolygonVertices();
        section.originalReinforcement.clearReinforcement();
        section.workingPolygon.clearPolygonVertices();

        // Gera os vértices para um retângulo centralizado na origem
        float halfBase = rectBase / 2.0f;
        float halfHeight = rectHeight / 2.0f;

        vector<Point> rectPoints = {
            {-halfBase, -halfHeight},
            { halfBase, -halfHeight},
            { halfBase,  halfHeight},
            {-halfBase,  halfHeight}
        };

        section.originalPolygon.setVertices(rectPoints);
        section.originalPolygon.SetNumPoints(rectPoints.size()); // Atualiza o número de pontos
        section.defineGeometry(section.originalPolygon, section.originalReinforcement);

        shouldAutoFit = true;
    }
}

void Interface::inputSectionT(Section &section)
{
    static float t_bf = 20.0f;
    static float t_hf = 5.0f;
    static float t_bw = 5.0f;
    static float t_hw = 20.0f;

    ImGui::SeparatorText("Dados da Seção T:");
    ImGui::InputFloat("Largura Mesa (bf cm)", &t_bf);
    ImGui::InputFloat("Altura Mesa (hf cm)", &t_hf);
    ImGui::InputFloat("Largura alma (bw cm)", &t_bw);
    ImGui::InputFloat("Altura alma (hw cm)", &t_hw);

    if (ImGui::Button("Gerar Seção T"))
    {
        section.originalPolygon.clearPolygonVertices();
        section.originalReinforcement.clearReinforcement();
        section.workingPolygon.clearPolygonVertices();

        float half_bf = t_bf / 2.0f;
        float top_y = t_hw + t_hf;
        float bottom_y_flange = t_hw;

        float half_bw = t_bw / 2.0f;
        float bottom_y_web = 0.0f;

        std::vector<Point> tPoints = 
        {
            {-half_bw, bottom_y_web },
            { half_bw, bottom_y_web },
            { half_bw, bottom_y_flange },
            { half_bf, bottom_y_flange },
            { half_bf, top_y },
            {-half_bf, top_y },
            {-half_bf, bottom_y_flange },
            {-half_bw, bottom_y_flange },
        };
        
        section.originalPolygon.setVertices(tPoints);
        section.originalPolygon.SetNumPoints(tPoints.size()); 
        section.defineGeometry(section.originalPolygon, section.originalReinforcement);

        shouldAutoFit = true;
    }
}

void Interface::inputSectionCircular(Section &section)
{
    static float circleRadius = 10.0f;
    static int circleSegments = 32;

    ImGui::SeparatorText("Dados Seção Circular:");
    ImGui::InputFloat("Raio (cm)", &circleRadius);
    ImGui::InputInt("Número de Segmentos", &circleSegments);

    if (circleSegments < 3) circleSegments = 3;

    if (ImGui::Button("Gerar Círculo"))
    {
        section.originalPolygon.clearPolygonVertices();

        std::vector<Point> circlePoints;

        float angleStep = (2.0f * M_PI) / circleSegments;

        for (int i = 0; i < circleSegments; ++i)
        {
            float angle = i * angleStep;
            float x = circleRadius * cos(angle);
            float y = circleRadius * sin(angle);
            circlePoints.push_back({x, y});
        }

        section.originalPolygon.setVertices(circlePoints);
        section.originalPolygon.SetNumPoints(circlePoints.size());
        section.defineGeometry(section.originalPolygon, section.originalReinforcement);

        shouldAutoFit = true;
    }   
}

void Interface::inputSectionPolygonal(Section &section)
{
    static int tempNumPoints = 0;

    tempNumPoints = section.originalPolygon.GetNumPoints();
    if (tempNumPoints < 0)
        tempNumPoints = 0;

    ImGui::SeparatorText("Quantidade de pontos do polígono:");
    ImGui::PushItemWidth(100);

    // InputInt com botões + e -. O 1 e 10 são os steps.
    if (ImGui::InputInt("##xx", &tempNumPoints, 1, 10))
    {
        if (tempNumPoints < 0) tempNumPoints = 0;
        section.originalPolygon.SetNumPoints(tempNumPoints);
    }
    ImGui::PopItemWidth(); // Pop do PushItemWidth

    ImGui::SeparatorText("Edição de Vértices:");

    if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        // Configura as colunas com larguras fixas e centralizadas
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("x (cm)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("y (cm)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        // Itera sobre todos os pontos do polígono
        for (int row = 0; row < section.originalPolygon.GetNumPoints(); row++)
        {
            ImGui::PushID(row);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0); // Coluna para 'ID'
            ImGui::Text("%d", row + 1);    // Exibe o índice do ponto (começa de 1)

            ImGui::TableSetColumnIndex(1); // Coluna para 'x'
            char labelX[10];
            snprintf(labelX, sizeof(labelX), "##xx%d", row); // ID único para cada InputFloat
            float x, y;
            section.originalPolygon.GetTableData(row, &x, &y); // Obter as coordenadas do ponto na linha 'row'

            // Cria um campo editável para a coordenada x
            if (ImGui::InputFloat(labelX, &x))
            {
                section.originalPolygon.SetTableData(row, x, y); // Atualiza a coordenada 'x' diretamente no vetor
                section.defineGeometry(section.originalPolygon, section.originalReinforcement);
            }

            ImGui::TableSetColumnIndex(2); // Coluna para 'y'
            char labelY[10];
            snprintf(labelY, sizeof(labelY), "##yy%d", row); // ID único para cada InputFloat

            // Cria um campo editável para a coordenada y
            if (ImGui::InputFloat(labelY, &y))
            {
                section.originalPolygon.SetTableData(row, x, y); // Atualiza a coordenada 'y' diretamente no vetor
                section.defineGeometry(section.originalPolygon, section.originalReinforcement);
            }

            ImGui::PopID(); // Remove o ID do ponto atual após a linha ter sido processada
        }

        ImGui::EndTable();
    }
    else 
    {
        // Mensagem para o usuário quando o modo é edição mas a tabela não é mostrada (ex: 0 pontos)
        if (section.originalPolygon.GetNumPoints() == 0) {
            ImGui::Text("Adicione pontos ou selecione uma seção padrão.");
        }
    }
}

void Interface::inputSectionDebug(Section &section)
{
    if (ImGui::Button("Seção T (Exemplo Atual)"))
    {
        section.originalPolygon.clearPolygonVertices();
        section.originalReinforcement.clearReinforcement();

        vector<Point> collectedPoints = {
            {7.5, 0}, {10, 30}, {20, 40}, {20, 50}, {-20, 50}, {-20, 40}, {-10, 30}, {-7.5, 0}};

        std::vector<Point> collectedReinf = {
            {5, 2.5}, {5, 7.5}, {-5, 7.5}, {-5, 2.5}};
        std::vector<double> collectedDiameters = {10, 10, 10, 10};

        section.originalPolygon.setVertices(collectedPoints);
        section.originalReinforcement.setReinforcement(collectedReinf, collectedDiameters);
        section.originalReinforcement.computeArea();

        section.defineGeometry(section.originalPolygon, section.originalReinforcement);

        shouldAutoFit = true;
    }
}

void Interface::showGeometricParameters(Section &section)
{
    ImGui::SeparatorText("Parâmetros Geométricos da Seção");   

    ImGui::Text("Area: %.2f (cm²) |", section.workingPolygon.getPolygonArea());
    ImGui::SameLine();
    ImGui::Text("Altura: %.2f (cm) |", section.workingPolygon.getMaxY() - section.workingPolygon.getMinY()); // Corrigi para usar getHeight
    ImGui::SameLine();
    ImGui::Text("CG: %.2f, %.2f", section.workingPolygon.getGeometricCenter().getX(), section.workingPolygon.getGeometricCenter().getY());

    ImGui::SeparatorText("Aviso");

    ImGui::BulletText("Contorno externo no sentido anti-horário.");
    ImGui::BulletText("Contorno interno no sentido horário.");
}

void Interface::showPopUpErrorPolygon()
{
    if (ImGui::BeginPopupModal("Vértices vazios", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Adicione a seção");

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            //showPopUpErrorPolygon = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Interface::clearInputSection(Section &section)
{
    if (ImGui::Button("Limpar"))
    {
        section.originalPolygon.clearPolygonVertices();
        section.workingPolygon.clearPolygonVertices();
        section.stressRegions.clearStressRegions();
        section.originalReinforcement.clearReinforcement();
        section.workingPolygon.clearPolygonVertices();
    }
}

void Interface::clearSection(Section &section)
{
    section.originalPolygon.clearPolygonVertices();
    section.workingPolygon.clearPolygonVertices();
    section.stressRegions.clearStressRegions();
    section.originalReinforcement.clearReinforcement();
    section.workingPolygon.clearPolygonVertices();
    section.envelopeMoments.clear();
    section.combinations.clear();
    section.concrete.setParameters(StressStrainConcreteModelType::PARABOLA_RECTANGLE_NBR6118_2023, 30.0, 1.4);
    section.steel.setParameters(StressStrainSteelModelType::PASSIVE_REINFORCEMENT, 500.0, 1.15, 210.0);
}

void Interface::interfaceMaterials(Section &section)
{
    if (ImGui::BeginMenu("Materiais"))
    {
        // Janela normal sendo aberta enquanto o menu estiver ativo
        ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Always); // Tamanho da janela
        ImGui::SetNextWindowPos(ImVec2(123, 47));                     // Posição na tela

        ImGui::Begin("Inserir Dados dos Materiais", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        if (ImGui::BeginTabBar("Tabela de Entrada de Dados de Materiais"))
        {
            if (ImGui::BeginTabItem("Concreto"))
            {
                concreteInterface(section);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Armadura Passiva"))
            {
                steelInterface(section);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Valores de Referência"))
            {
                ReferenceValues();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End(); // Fecha janela

        ImGui::EndMenu(); // Fecha menu
    }
}

void Interface::concreteInterface(Section &section)
{
    static int constitutiveModel = 1;
    static double collectedFck = 0.0, collectedGammaC = 0.0, stress;
    int x, y;

    collectedFck = section.concrete.getFck();
    collectedGammaC = section.concrete.getGammaC();

    ImGui::RadioButton("NBR 6118:2023", &constitutiveModel, 1);
    ImGui::SameLine();
    ImGui::RadioButton("NBR 6118:2014", &constitutiveModel, 0);

    if (constitutiveModel == 0)

    {
        StressStrainConcreteModelType model61182014 = StressStrainConcreteModelType::PARABOLA_RECTANGLE_NBR6118_2014;

        ImGui::PushItemWidth(70);
        ImGui::SetCursorPos(ImVec2(650, 70)); // Define a posição do cursor
        ImGui::BeginGroup();
        ImGui::Text("Parâmetros do Concreto");
        ImGui::InputDouble("fck (MPa):", &collectedFck, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("γc: ", &collectedGammaC, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (collectedFck < 0 || collectedGammaC < 0)
        {
            collectedFck = 30;
            collectedGammaC = 1.4;
        }

        if (collectedFck > 90)
        {
            collectedFck = 90;
        }

        section.concrete.setParameters(model61182014, collectedFck, collectedGammaC);
        section.concrete.setCurveStressStrain();

        ImVec2 plotSize = ImGui::GetContentRegionAvail();
        ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual
        applyDarkElegantPlotStyle();
        // inicialização do gráfico com os eixos
        ImGui::SetCursorPos(ImVec2(0, 70)); // Define a posição do cursor
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação concreto NBR 6118:2014", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxis(ImAxis_X1, " ε ‰");
            ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);

            renderStrainConcreteRuptureDiagram(section.concrete.getCurveStressStrain(), "TensaoxDef");
            // renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");

            double x_values[] = {0, section.concrete.getStrainConcretePlastic()};
            double y_values[] = {section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_values, y_values, 2);

            ImPlot::Annotation(0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), ImVec4(1, 0, 1, 0), ImVec2(0, -1), section.concrete.getStrainConcretePlastic(), " (β * fcd)  = %.2f MPa", section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()));
            double x_ec2[] = {section.concrete.getStrainConcretePlastic(), section.concrete.getStrainConcretePlastic()};
            double y_ec2[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ec2, y_ec2, 2);

            double x_ecu[] = {section.concrete.getStrainConcreteRupture(), section.concrete.getStrainConcreteRupture()};
            double y_ecu[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ecu, y_ecu, 2);

            ImPlot::Annotation(section.concrete.getStrainConcretePlastic(), 0, ImVec4(1, 0, 1, 0), ImVec2(-10, 0), section.concrete.getStrainConcretePlastic(), "%.2f‰", section.concrete.getStrainConcretePlastic());
            ImPlot::Annotation(section.concrete.getStrainConcreteRupture(), 0, ImVec4(1, 0, 1, 0), ImVec2(10, 0), section.concrete.getStrainConcreteRupture(), "%.2f‰", section.concrete.getStrainConcreteRupture());
            ImPlot::EndPlot();
        }
        ImPlot::GetStyle() = backup; // restaura estilo anterior
    }

    if (constitutiveModel == 1)
    {
        StressStrainConcreteModelType model61182023 = StressStrainConcreteModelType::PARABOLA_RECTANGLE_NBR6118_2023;

        ImGui::PushItemWidth(70);
        ImGui::SetCursorPos(ImVec2(650, 70)); // Define a posição do cursor
        ImGui::BeginGroup();
        ImGui::Text("Parâmetros do Concreto");
        ImGui::InputDouble("fck (MPa):", &collectedFck, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("γc:", &collectedGammaC, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (collectedFck < 0 || collectedGammaC < 0)
        {
            collectedFck = 30;
            collectedGammaC = 1.4;
        }

        if (collectedFck > 90)
        {
            collectedFck = 90;
        }

        section.concrete.setParameters(model61182023, collectedFck, collectedGammaC);
        section.concrete.setCurveStressStrain();

        ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual

        applyDarkElegantPlotStyle();
        ImVec2 plotSize = ImGui::GetContentRegionAvail();

        ImGui::SetCursorPos(ImVec2(0, 70)); // Define a posição do cursor
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação concreto NBR 6118:2023", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxis(ImAxis_X1, " ε ‰");
            ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);

            renderStrainConcreteRuptureDiagram(section.concrete.getCurveStressStrain(), "TensaoxDef");
            // renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");

            double x_values[] = {0, section.concrete.getStrainConcretePlastic()};
            double y_values[] = {section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_values, y_values, 2);

            ImPlot::Annotation(0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), ImVec4(1, 0, 1, 0), ImVec2(0, -1), section.concrete.getStrainConcretePlastic(), " (β * ηc * fcd)  = %.2f MPa", section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()));
            double x_ec2[] = {section.concrete.getStrainConcretePlastic(), section.concrete.getStrainConcretePlastic()};
            double y_ec2[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ec2, y_ec2, 2);

            double x_ecu[] = {section.concrete.getStrainConcreteRupture(), section.concrete.getStrainConcreteRupture()};
            double y_ecu[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ecu, y_ecu, 2);

            ImPlot::Annotation(section.concrete.getStrainConcretePlastic(), 0, ImVec4(1, 0, 1, 0), ImVec2(-10, 0), section.concrete.getStrainConcretePlastic(), "%.2f‰", section.concrete.getStrainConcretePlastic());
            ImPlot::Annotation(section.concrete.getStrainConcreteRupture(), 0, ImVec4(1, 0, 1, 0), ImVec2(10, 0), section.concrete.getStrainConcreteRupture(), "%.2f‰", section.concrete.getStrainConcreteRupture());
            ImPlot::EndPlot();
        }
        ImPlot::GetStyle() = backup; // restaura estilo anterior
    }
}

void Interface::steelInterface(Section &section)
{

    std::vector<Point> annotationPoints;

    annotationPoints.push_back(Point(-10, section.steel.computeStress(-section.steel.getStrainSteelYield())));
    annotationPoints.push_back(Point(-section.steel.getStrainSteelYield(), section.steel.computeStress(-section.steel.getStrainSteelYield())));
    annotationPoints.push_back(Point(section.steel.getStrainSteelYield(), section.steel.computeStress(section.steel.getStrainSteelYield())));
    annotationPoints.push_back(Point(10, section.steel.computeStress(section.steel.getStrainSteelYield())));

    static double collectedFyk = 0.0, collectedGammaS = 0.0, collectedE = 0.0, stress;

    collectedFyk = section.steel.getFyk();
    collectedGammaS = section.steel.getGammaS();
    collectedE = section.steel.getE();

    ImGui::PushItemWidth(70);
    ImGui::SetCursorPos(ImVec2(650, 70)); // Define a posição do cursor
    ImGui::BeginGroup();
    ImGui::Text("Parâmetros da Armadura Passiva");
    ImGui::InputDouble("fyk (MPa):", &collectedFyk, 0.0f, 0.0f, "%.3f");
    ImGui::InputDouble("γs: ", &collectedGammaS, 0.0f, 0.0f, "%.3f");
    ImGui::InputDouble("E (GPa):", &collectedE, 0.0f, 0.0f, "%.3f");
    ImGui::EndGroup();

    if (collectedFyk < 0 || collectedGammaS < 0 || collectedE < 0)
    {
        collectedFyk = 500;
        collectedGammaS = 1.15;
        collectedE = 210;
    }

    if (collectedFyk > 1000)
    {
        collectedFyk = 1000;
    }

    StressStrainSteelModelType modelPassive = StressStrainSteelModelType::PASSIVE_REINFORCEMENT;
    section.steel.setParameters(modelPassive, collectedFyk, collectedGammaS, collectedE);
    section.steel.setCurveStressStrain();

    ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual

    applyDarkElegantPlotStyle();

    ImVec2 plotSize = ImGui::GetContentRegionAvail();
    ImGui::SetCursorPos(ImVec2(0, 70)); // Define a posição do cursor
    // inicialização do gráfico com os eixos
    if (ImPlot::BeginPlot("Diagrama Tensão-Deformação aço NBR 6118:2023", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxis(ImAxis_X1, " ε ‰ ");
        ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
        ImPlot::SetupAxesLimits((-section.steel.getStrainSteelRupture() * 1.1), (section.steel.getStrainSteelRupture() * 1.1),
                                (-section.steel.getFyd() * 1.5), (section.steel.getFyd() * 1.5), ImGuiCond_Always);
        renderStrainSteelDiagram(annotationPoints, "TensaoxDef");

        ImPlot::Annotation(-section.steel.getStrainSteelYield(), section.steel.computeStress(-section.steel.getStrainSteelYield()), ImVec4(1, 1, 1, 0), ImVec2(-10, 2), section.steel.computeStress(-section.steel.getStrainSteelYield()), "fyd = %.2f MPa", section.steel.computeStress(-section.steel.getStrainSteelYield()));
        ImPlot::Annotation(section.steel.getStrainSteelYield(), section.steel.computeStress(section.steel.getStrainSteelYield()), ImVec4(1, 1, 1, 0), ImVec2(10, -2), section.steel.computeStress(section.steel.getStrainSteelYield()), "fyd = %.2f MPa", section.steel.computeStress(section.steel.getStrainSteelYield()));

        ImPlot::Annotation(-section.steel.getStrainSteelYield(), 0, ImVec4(1, 1, 1, 0), ImVec2(-2, -1), -section.steel.getStrainSteelYield(), "%.2f‰", -section.steel.getStrainSteelYield());
        ImPlot::Annotation(section.steel.getStrainSteelYield(), 0, ImVec4(1, 1, 1, 0), ImVec2(2, 1), section.steel.getStrainSteelYield(), "%.2f‰", section.steel.getStrainSteelYield());

        double x_eyd[] = {-section.steel.getStrainSteelYield(), -section.steel.getStrainSteelYield()};
        double y_eyd[] = {0, -section.steel.computeStress(section.steel.getStrainSteelYield())};
        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
        ImPlot::PlotLine("", x_eyd, y_eyd, 2, IM_COL32(255, 255, 255, 255));

        double x_eyd2[] = {section.steel.getStrainSteelYield(), section.steel.getStrainSteelYield()};
        double y_eyd2[] = {0, section.steel.computeStress(section.steel.getStrainSteelYield())};
        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
        ImPlot::PlotLine("", x_eyd2, y_eyd2, 2, IM_COL32(255, 255, 255, 255));

        // ImPlot::Annotation(reforco.epsilon_yd, tensaoY[3], ImVec4(1, 1, 1, 0), ImVec2(annotation_offset_x, 1), tensaoY[3], "fyd = %.2f MPa", tensaoY[3]);
        ImPlot::EndPlot();
    }

    ImPlot::GetStyle() = backup; // restaura estilo anterior
}

void Interface::reinforcementInterface(Section &section)
{
    if (ImGui::BeginMenu("Armadura"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 420), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(191, 47));

        static int barMode = 0, numBar = 0, tempNumPoints = 0;
        static double coordXBar, coordYBar, diameterBar = 10;
        static double coordXiBar, coordXfBar, coordYiBar, coordYfBar;
        static bool showPopUpErrorBar = false;

        if (section.originalReinforcement.getReinforcement().empty())
            tempNumPoints = 0; // Se o vetor de armadura estiver vazio, o número de pontos temporário é 0
        else
            tempNumPoints = section.originalReinforcement.getReinforcement().size(); // Atualiza o número de pontos temporário com o tamanho do vetor de armadura

        ImGui::Begin("Inserir Dados da Armadura Passiva", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        ImGui::SeparatorText("Modo de Inserção");
        ImGui::RadioButton("Uma barra", &barMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Linha de barras", &barMode, 1);

        if (barMode == 0)
        {
            ImGui::SeparatorText("Barra Individual");

            ImGui::PushItemWidth(100);
            ImGui::InputInt("Número de barras", &tempNumPoints);
            if (tempNumPoints < 0)
                tempNumPoints = 0;

            if (tempNumPoints != section.originalReinforcement.getReinforcement().size())
            {
                section.originalReinforcement.SetNumPoints(tempNumPoints);
                section.defineReinforcement(section.originalReinforcement);            
            }

            ImGui::SameLine();
            if (ImGui::Button("Limpar Tudo"))
            {
                section.originalReinforcement.clearReinforcement();
                section.defineReinforcement(section.originalReinforcement);            
            }
        }

        if (barMode == 1)
        {
            ImGui::PushID(1); // Garante que o ID seja único para cada barra
            ImGui::PushItemWidth(100);
            ImGui::BeginGroup();
            ImGui::SeparatorText("Quantidade de barras:");
            ImGui::InputInt("Número de barras", &tempNumPoints);
            if (tempNumPoints < 0)
                tempNumPoints = 0;

            if (tempNumPoints != section.originalReinforcement.getReinforcement().size())
            {
                section.originalReinforcement.SetNumPoints(tempNumPoints);
                section.defineReinforcement(section.originalReinforcement);
            }

            ImGui::SameLine();
            if (ImGui::Button("Limpar Tudo"))
                section.originalReinforcement.clearReinforcement();

            ImGui::PopID(); // Remove o ID do ponto atual após a linha ter sido processada

            ImGui::SeparatorText("Linha de Barras:");
            ImGui::PushID(2); // Garante que o ID seja único para cada barra
            ImGui::InputInt("Número de barras", &numBar);
            if (numBar < 2)
                numBar = 2;
            ImGui::InputDouble("Diâmetro das barras (mm)", &diameterBar, 0.0, 0.0, "%.2f");
            ImGui::InputDouble("xi (cm)", &coordXiBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::InputDouble("xf (cm)", &coordXfBar, 0.0, 0.0, "%.2f");
            ImGui::InputDouble("yi (cm)", &coordYiBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::InputDouble("yf (cm)", &coordYfBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::PopID();
            ImGui::EndGroup();

            if (diameterBar < 0)
            {
                diameterBar = 0; // Valor padrão se o diâmetro for negativo
            }

            if (ImGui::Button("Adicionar Linha"))
            {
                if (diameterBar > 0)
                {
                    double stepX = (coordXfBar - coordXiBar) / (numBar - 1);
                    double stepY = (coordYfBar - coordYiBar) / (numBar - 1);
                    for (int i = 0; i < numBar; ++i)
                    {
                        double x = coordXiBar + stepX * i;
                        double y = coordYiBar + stepY * i;
                        section.originalReinforcement.addReinforcement(x, y, diameterBar);
                    }
                    section.originalReinforcement.computeArea();
                    section.defineReinforcement(section.originalReinforcement);
                }
                else
                    showPopUpErrorBar = true;
            }
        }

        // Tabela com dados das barras
        if (ImGui::BeginTable("TabelaBarras", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("x (cm)");
            ImGui::TableSetupColumn("y (cm)");
            ImGui::TableSetupColumn("Diâmetro (mm)");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < section.originalReinforcement.GetNumPoints(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", static_cast<int>(i + 1)); // ID da barra

                ImGui::TableSetColumnIndex(1);
                char labelX[10];
                snprintf(labelX, sizeof(labelX), "##x%d", i); // Cria o label para cada coordenada X
                double x, y, d;
                section.originalReinforcement.GetTableData(i, &x, &y, &d); // Obter as coordenadas do ponto na linha 'row'

                if (ImGui::InputDouble(labelX, &x, 0.0, 0.0, "%.2f")) // Cria um campo editável para a coordenada x
                {
                    section.originalReinforcement.SetTableData(i, x, y, d);                        // Atualiza a coordenada 'x' diretamente no vetor
                    section.defineReinforcement(section.originalReinforcement); // Atualiza a geometria da seção
                }
                ImGui::TableSetColumnIndex(2); // Coluna para 'y'
                char labelY[10];
                snprintf(labelY, sizeof(labelY), "##y%d", i); // Cria o label para cada coordenada Y

                if (ImGui::InputDouble(labelY, &y, 0.0, 0.0, "%.2f")) // Cria um campo editável para a coordenada y
                {
                    section.originalReinforcement.SetTableData(i, x, y, d);                        // Atualiza a coordenada 'y' diretamente no vetor
                    section.defineReinforcement(section.originalReinforcement); // Atualiza a geometria da seção
                }

                ImGui::TableSetColumnIndex(3); // Coluna para 'Diâmetro'
                char labelD[10];
                snprintf(labelD, sizeof(labelD), "##d%d", i);         // Cria o label para cada diâmetro
                if (ImGui::InputDouble(labelD, &d, 0.0, 0.0, "%.2f")) // Cria um campo editável para o diâmetro
                {
                    if (d > 0)
                    {
                        section.originalReinforcement.SetTableData(i, x, y, d);                        // Atualiza o diâmetro diretamente no vetor
                        section.originalReinforcement.computeArea();                                   // Recalcula a área da armadura
                        section.defineReinforcement(section.originalReinforcement); // Atualiza a geometria da seção
                    }
                    else
                    {
                        showPopUpErrorBar = true; // Exibe popup de erro se o diâmetro for inválido
                    }
                }
                ImGui::PopID(); // Remove o ID do ponto atual após a linha ter sido processada
            }

            ImGui::EndTable();
        }

        // Popup de erro
        if (showPopUpErrorBar)
        {
            ImGui::OpenPopup("Erro de Entrada");
        }

        if (ImGui::BeginPopupModal("Erro de Entrada", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Diâmetro inválido. Insira um valor positivo.");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showPopUpErrorBar = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End(); // Fecha janela
        ImGui::EndMenu();
    }
}

void Interface::ReferenceValues()
{
    ImGui::Text("Deslize o cursor sobre os valores para mais informações.");
    ImGui::Separator();
    if (ImGui::BeginTable("valores_normativos", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Material");
        ImGui::TableSetupColumn("Parâmetro");
        ImGui::TableSetupColumn("Valor Mínimo");
        ImGui::TableSetupColumn("Valor Padrão");
        ImGui::TableSetupColumn("Valor Máximo");
        ImGui::TableHeadersRow();

        // ----- CONCRETO -----
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Concreto");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("fck (MPa)");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("20");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Resistência mínima característica à compressão para concreto estrutural.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("25 a 50");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Faixa mais comum em projetos usuais de estruturas de concreto armado.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("90");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor máximo aceito pela NBR 6118 para uso direto no diagrama tensão-deformação.");
            ImGui::EndTooltip();
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Concreto");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("γc");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("1.2");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Pode ser adotado em avaliações de estruturas existentes ou reforços.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("1.4");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor padrão para segurança do concreto em estado limite último (ELU).");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("1.5");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Usado em casos excepcionais com grande incerteza nos materiais.");
            ImGui::EndTooltip();
        }

        // ----- AÇO -----
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Aço");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("fyk (MPa)");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("250");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Aço CA-25, pouco usado atualmente.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("500");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Aço CA-50, o mais utilizado no Brasil.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("600");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Aço CA-60, usado em estruturas com alta exigência de resistência.");
            ImGui::EndTooltip();
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Aço");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("γs");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("1.0");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Adotado em avaliações de estruturas existentes com dados confiáveis.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("1.15");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor padrão da norma para dimensionamento no ELU.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("1.2");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Usado raramente, para situações de alta incerteza.");
            ImGui::EndTooltip();
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Aço");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("E (GPa)");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("200");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor inferior típico de alguns aços especiais.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("210");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor normativo para o módulo de elasticidade do aço CA-50/60.");
            ImGui::EndTooltip();
        }

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("215");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Valor superior teórico em casos específicos.");
            ImGui::EndTooltip();
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Aplicações Típicas por Classe de Concreto");

    ImGui::BulletText("fck = 20 a 30 MPa: residências, pequenas edificações");
    ImGui::BulletText("fck = 35 a 50 MPa: prédios comerciais, estruturas padrão");
    ImGui::BulletText("fck > 50 MPa: pilares esbeltos, pontes, grandes vãos");

    ImGui::Separator();
    ImGui::TextWrapped("Nota: Os valores e fórmulas seguem as recomendações da NBR 6118:2023, podendo variar conforme o tipo estrutural e critérios de segurança do projeto.");
}

void Interface::effortSectionInterface(Section &section)
{
    if (ImGui::BeginMenu("Esforços"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(265, 47));                     // Posição inicial
        static double Nsd, Mx, My, eps1, eps2, angle;
        static bool showPopUpErrorAxialForce = false;
        static bool showPopUpSolver = false;
        static int tempNumCombinations = 1;
        static bool showPopUpErrorPolygon = false;
        static bool showPopUpErrorBar = false;

        ImGui::Begin("Entrada de Dados: Esforços", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::PushItemWidth(100);
        ImGui::SeparatorText("Número de combinações de esforços");

        if (section.combinations.empty())
        {
            section.combinations.resize(1, Combination(0.0f, 0.0f, 0.0f, 0.0f, false));
            mappingID.resize(1);
        }

        if (ImGui::InputInt("##XX:", &tempNumCombinations))
        {
            if (tempNumCombinations < 1)
                tempNumCombinations = 1;

            section.combinations.resize(tempNumCombinations, Combination(0.0f, 0.0f, 0.0f, 0.0f, false));
            mappingID.resize(tempNumCombinations);
        }

        if (ImGui::BeginTable("TabelaEsforcos", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Nsd (kN)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("M,x (kN.m)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("M,y (kN.m)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();

            for (int i = 0; i < section.combinations.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();

                float nsd = section.combinations[i].Normal;
                float mx = section.combinations[i].MsdX;
                float my = section.combinations[i].MsdY;

                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);

                // Nsd
                ImGui::TableSetColumnIndex(1);
                char labelN[16];
                snprintf(labelN, sizeof(labelN), "##nsd%d", i);
                if (ImGui::InputFloat(labelN, &nsd))
                    section.combinations[i].Normal = nsd;

                // Mx
                ImGui::TableSetColumnIndex(2);
                char labelMx[16];
                snprintf(labelMx, sizeof(labelMx), "##mx%d", i);
                if (ImGui::InputFloat(labelMx, &mx))
                    section.combinations[i].MsdX = mx;

                // My
                ImGui::TableSetColumnIndex(3);
                char labelMy[16];
                snprintf(labelMy, sizeof(labelMy), "##my%d", i);
                if (ImGui::InputFloat(labelMy, &my))
                    section.combinations[i].MsdY = my;

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Limpar"))
        {
            if (section.combinations.size() > 0)
                section.combinations.clear();
            if (section.envelopeMoments.size() > 0)
                section.envelopeMoments.clear();
            tempNumCombinations = 1;
        }

        ImGui::SameLine();

        if (ImGui::Button("Calcular"))
        {

            bool hasPolygon = !section.workingPolygon.getPolygonVertices().empty();
            bool hasReinforcement = !section.workingReinforcement.getReinforcement().empty();

            if (!hasPolygon)
            {
                showPopUpErrorPolygon = true;
            }
            else if (!hasReinforcement)
            {
                showPopUpErrorBar = true;
            }

            if (section.workingPolygon.getPolygonVertices().empty() || section.workingReinforcement.getReinforcement().empty())
            {
                ImGui::OpenPopup("Erro de Esforço Normal");
                showPopUpErrorAxialForce = true;
            }
            else
            {
                section.defineGeometry(section.originalPolygon, section.originalReinforcement);
                section.defineMaterials(section.concrete, section.steel);
                section.internalForces.computeMaxCompression(section.workingPolygon, section.workingReinforcement, section.steel, section.concrete);
                section.internalForces.computeMaxTraction(section.workingPolygon, section.workingReinforcement, section.steel);

                for (int i = 0; i < section.combinations.size(); ++i)
                {
                    section.internalForces.setNormalSolicitation(section.combinations[i].Normal);

                    if (section.internalForces.getNormalSolicitation() < section.internalForces.getMaxNormalCompression() || section.internalForces.getNormalSolicitation() > section.internalForces.getMaxNormalTraction())
                        section.combinations[i].isMomentValid = false;

                    else
                    {
                        section.computeEnvelope(section.combinations[i].Normal);
                        section.combinations[i].MsolverXX = section.momentSolver.getMoment().getX();
                        section.combinations[i].MsolverYY = section.momentSolver.getMoment().getY();
                        section.combinations[i].isMomentValid = true;
                        section.combinations[i].isCalculated = true;
                        shouldAutoFit = true;
                        shouldAutoFitEnv = true;

                        for (size_t j = 0; j < mappingID.size(); ++j)
                            mappingID[j] = false; // Limpa o mapeamento antes de selecionar
                    }
                }
            }

        }

        if (showPopUpErrorAxialForce)
        {
            if (ImGui::BeginPopupModal("Erro de Esforço Normal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("O esforço normal solicitante está fora do intervalo resistente da seção.");
                ImGui::Separator();
                ImGui::Text("Nsd: %.2f", Nsd);
                ImGui::Text("Intervalo permitido:");
                ImGui::BulletText("Máx. Compressão: %.2f kN", section.internalForces.getMaxNormalCompression());
                ImGui::BulletText("Máx. Tração: %.2f kN", section.internalForces.getMaxNormalTraction());

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpErrorAxialForce = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        if (showPopUpErrorPolygon == true)
        {
            ImGuiIO &io = ImGui::GetIO();

            ImGui::OpenPopup("Erro de inserção de dados");

            // Define a posição para o centro da tela
            ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("O polígono não foi definido corretamente.");
                ImGui::Separator();

                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpErrorPolygon = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        if (showPopUpErrorBar == true)
        {
            ImGuiIO &io = ImGui::GetIO();

            ImGui::OpenPopup("Erro de inserção de dados");

            // Define a posição para o centro da tela
            ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("A armadura não foi definida corretamente.");
                ImGui::Separator();

                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpErrorBar = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        /* if (showPopUpSolver)
        {
            if (ImGui::BeginPopupModal("Cálculo do Momento Resistente", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("MrdXX: %.2f (kN.m)", section.momentSolver.getMoment().getX());
                ImGui::Text("MrdYY: %.2f (kN.m)", section.momentSolver.getMoment().getY());
                ImGui::Separator();
                ImGui::Text("Valor de ε1: %.4f", section.momentSolver.getTopFiberStrain());
                ImGui::Text("Valor de ε2: %.4f", section.momentSolver.getBottomFiberStrain());
                ImGui::Separator();
                ImGui::Text("Iteracoes: %d", section.momentSolver.getIterations());

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpSolver = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }
            */
        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::crossSectionPlotInterface(Section &section, float posY)
{
    ImGuiIO &io = ImGui::GetIO();

    float largura = (io.DisplaySize.x - 300.0f)* 0.5; // 50% da largura total da tela
    float alturaDisponivel = io.DisplaySize.y - posY;

    ImGui::SetNextWindowPos(ImVec2(0, posY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(largura, alturaDisponivel), ImGuiCond_Always);

    ImGui::Begin("Grafico da Secao Transversal",
                 nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual

    applyDarkElegantPlotStyle();
    
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("Gráfico da Seção Transversal", ImVec2(plotSize.x, plotSize.y), 
    ImPlotFlags_Equal | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend | ImPlotFlags_NoInputs))
    {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.1f, 0.1f));

        if (section.workingPolygon.getPolygonVertices().size() > 2)
        {
            if (shouldAutoFit)
            {
                autoFitToPointsWithMargin(section.workingPolygon.getPolygonVertices(), 0.1f);
 
                shouldAutoFit = false;
            }

            renderPolygon(section.workingPolygon.getPolygonVertices(), "Vertices", "Polygon");
            renderPolygon(section.stressRegions.getCompressedRegion().getPolygonVertices(), "vComp", "pComp");
            renderPolygon(section.stressRegions.getParabolicRegion().getPolygonVertices(), "vParab", "pParab");
            renderPolygon(section.stressRegions.getRectangularRegion().getPolygonVertices(), "vRec", "pRec");
            //renderVectorPoint(section.workingReinforcement.getReinforcement(), "Barras");
            renderReinforcement(section, "Barras");
        }

        ImPlot::EndPlot();
    }
    ImPlot::GetStyle() = backup; // restaura estilo anterior
    ImGui::End();
}

void Interface::envelopeMomentsPlotInterface(Section &section, float posY)
{
    ImGuiIO &io = ImGui::GetIO();

    float largura = (io.DisplaySize.x - 300.0f) * 0.5; // 50% da largura total da tela
    float alturaDisponivel = io.DisplaySize.y - posY;
    float posicaoX = (largura); // Centraliza a janela horizontalmente

    ImGui::SetNextWindowPos(ImVec2(posicaoX, posY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(largura,  alturaDisponivel), ImGuiCond_Always);

    ImGui::Begin("Envoltoria", nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual

    applyDarkElegantPlotStyle();
    
    ImVec2 plotSize = ImGui::GetContentRegionAvail();
    
    if (ImPlot::BeginPlot("Envoltoria de Momentos Resistentes", ImVec2(plotSize.x, plotSize.y), 
    ImPlotFlags_Equal | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend | ImPlotFlags_NoInputs))
    {
        ImPlot::SetupAxis(ImAxis_X1, "MsdY (kN.m)", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, "MsdX (kN.m)", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Invert);

        ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.1f, 0.1f));

        if (shouldAutoFitEnv)
        {
            autoFitToPointsWithMargin(section.getEnvelopeMoments(), 0.1f);
            shouldAutoFitEnv = false;
        }

        renderPolygon(section.envelopeMoments, "Vertices", "Envoltoria");
        
        if (section.combinations.size() > 0)
        {
            for (size_t i = 0; i < mappingID.size(); i++)
            {
                if (mappingID[i] == true)
                {
                    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 5, ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
                    std::vector<Point> singlePointVector = {section.combinations[i].getMsd()};
                    renderVectorPoint(singlePointVector, "Msd Solicitante");
                }
            }
        }

        ImPlot::EndPlot();
    }
    ImPlot::GetStyle() = backup; // restaura estilo anterior
    ImGui::End();
}

void Interface::renderPolygon(const vector<Point> &polygonVertices, string nameVertices, string namePolygon)
{
    if (!polygonVertices.empty())
    {
        vector<double> xTemp(polygonVertices.size());
        vector<double> yTemp(polygonVertices.size());

        for (size_t i = 0; i < polygonVertices.size(); i++)
        {
            xTemp[i] = polygonVertices[i].getX();
            yTemp[i] = polygonVertices[i].getY();
        }

        if (polygonVertices.size() > 2)
        {
            vector<double> xTempEdge(polygonVertices.size() + 1);
            vector<double> yTempEdge(polygonVertices.size() + 1);

            for (size_t i = 0; i < polygonVertices.size(); i++)
            {
                xTempEdge[i] = polygonVertices[i].getX();
                yTempEdge[i] = polygonVertices[i].getY();
            }

            xTempEdge[polygonVertices.size()] = polygonVertices[0].getX();
            yTempEdge[polygonVertices.size()] = polygonVertices[0].getY();

            ImPlot::PlotLine(namePolygon.c_str(), xTempEdge.data(), yTempEdge.data(), static_cast<int>(xTempEdge.size()));
        }
    }
}

void Interface::renderVectorPoint(const vector<Point> &vectorPoint, string nameVectorPoint)
{
    if (!vectorPoint.empty())
    {
        vector<double> xTemp(vectorPoint.size());
        vector<double> yTemp(vectorPoint.size());

        for (size_t i = 0; i < vectorPoint.size(); i++)
        {
            xTemp[i] = vectorPoint[i].getX();
            yTemp[i] = vectorPoint[i].getY();
        }

        ImPlot::PlotScatter(nameVectorPoint.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()));
    }
}

void Interface::renderStrainConcreteRuptureDiagram(const vector<Point> &vectorPoint, string nameVectorPoint)
{
    if (!vectorPoint.empty())
    {
        vector<double> xTemp(vectorPoint.size());
        vector<double> yTemp(vectorPoint.size());

        for (size_t i = 0; i < vectorPoint.size(); i++)
        {
            xTemp[i] = vectorPoint[i].getX();
            yTemp[i] = vectorPoint[i].getY();
        }

        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.0f, 0.58f, 0.90f, 0.3f)); // azul com transparência
        ImPlot::PlotShaded((nameVectorPoint + "_fill").c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()), 0.0);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 0.58f, 0.90f, 1.0f)); // RGB(0, 150, 255)
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.5f);
        ImPlot::PlotLine(nameVectorPoint.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()));
        ImPlot::PopStyleVar();
        ImPlot::PopStyleColor();
    }
}

void Interface::renderStrainSteelDiagram(const vector<Point> &vectorPoint, string nameVectorPoint)
{

    if (!vectorPoint.empty())
    {
        vector<double> xTemp(vectorPoint.size());
        vector<double> yTemp(vectorPoint.size());

        for (size_t i = 0; i < vectorPoint.size(); i++)
        {
            xTemp[i] = vectorPoint[i].getX();
            yTemp[i] = vectorPoint[i].getY();
        }

        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.0f, 0.58f, 0.90f, 0.3f)); // azul com transparência
        ImPlot::PlotShaded((nameVectorPoint + "_fill").c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()), 0.0);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 0.58f, 0.90f, 1.0f)); // RGB(0, 150, 255)
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.5f);
        ImPlot::PlotLine(nameVectorPoint.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()));
        ImPlot::PopStyleVar();
        ImPlot::PopStyleColor();
    }
}

void Interface::EffortsTable(Section &section)
{
    static int selectedEffort = -1;
    static bool showPopUpSolver = false;
    static bool showPopUpErrorAxialForce = false;
    static bool showPopUpErrorPolygon = false;
    static bool showPopUpErrorBar = false;

    if (ImGui::BeginTable("Tabela", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Nsd (kN)", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("M,x (kN.m)", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("M,y (kN.m)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < section.combinations.size(); ++i)
        {
            ImGui::TableNextRow();

            // Coluna 0 - ID (com Selectable para ativar popup)
            ImGui::TableSetColumnIndex(0);
            std::string rowId = "##row" + std::to_string(i);
            if (ImGui::Selectable(rowId.c_str(), selectedEffort == (int)i, ImGuiSelectableFlags_SpanAllColumns))
            {
                selectedEffort = static_cast<int>(i);
                
                for (size_t j = 0; j < mappingID.size(); ++j)
                    mappingID[j] = false; 
                    
                mappingID[i] = true;

                section.computeEnvelope(section.combinations[i].Normal);

                if (section.combinations[i].isMomentValid)
                {
                }
                else
                    showPopUpErrorAxialForce = true;

            }

            ImGui::SameLine();
            ImGui::Text("%d", static_cast<int>(i + 1));

            // Coluna 1 - Nsd
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f", section.combinations[i].Normal); 
            // if (section.combinations[i].isCalculated)
            // {
            //     if (!section.combinations[i].isMomentValid)
            //     {
            //         ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); // Vermelho se inválido
            //         ImGui::Text("%.2f", section.combinations[i].Normal);
            //         ImGui::PopStyleColor();
            //     }
            //     else
            //     {
            //         ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // Verde se válido
            //         ImGui::Text("%.2f", section.combinations[i].Normal);
            //         ImGui::PopStyleColor();
            //     }
            // }
            // else
            // {
            //     ImGui::Text("%.2f", section.combinations[i].Normal); 
            // }

            // Coluna 2 - MsdX
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.2f", section.combinations[i].MsdX);

            // Coluna 3 - M. Resistente (com destaque em vermelho se inválido)
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.2f", section.combinations[i].MsdY);
        }

        ImGui::EndTable();
    }

    if (showPopUpErrorPolygon == true)
    {
        ImGuiIO &io = ImGui::GetIO();

        ImGui::OpenPopup("Erro de inserção de dados");

        // Define a posição para o centro da tela
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("O polígono não foi definido corretamente.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
            if (ImGui::Button("OK##PopUpErrorPolygon", ImVec2(120, 0)))
            {
                showPopUpErrorPolygon = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    if (showPopUpErrorBar == true)
    {
        ImGuiIO &io = ImGui::GetIO();

        ImGui::OpenPopup("Erro de inserção de dados");

        // Define a posição para o centro da tela
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("A armadura não foi definida corretamente.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
            if (ImGui::Button("OK##PopUpErrorBar", ImVec2(120, 0)))
            {
                showPopUpErrorBar = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    // --- POPUP: Momento resistente calculado com sucesso
    if (showPopUpSolver && selectedEffort >= 0)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImVec2 posjanela = ImVec2(io.DisplaySize.x - 260, io.DisplaySize.y / 3.0f);
        ImGui::OpenPopup("Cálculo do Momento Resistente");
        ImGui::SetNextWindowPos(posjanela, ImGuiCond_Always);
        if (ImGui::BeginPopupModal("Cálculo do Momento Resistente", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("MrdXX: %.2f (kN.m)", section.momentSolver.getMoment().getX());
            ImGui::Text("MrdYY: %.2f (kN.m)", section.momentSolver.getMoment().getY());
            ImGui::Separator();
            ImGui::Text("Valor de ε 1: %.4f", section.momentSolver.getTopFiberStrain());
            ImGui::Text("Valor de ε 2: %.4f", section.momentSolver.getBottomFiberStrain());
            ImGui::Separator();
            ImGui::Text("Iterações: %d", section.momentSolver.getIterations());

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showPopUpSolver = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }


    if (showPopUpErrorAxialForce && selectedEffort >= 0)
    {

        ImGuiIO &io = ImGui::GetIO();

        ImGui::OpenPopup("Erro de esforço normal");

        // Define a posição para o centro da tela
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro de Esforço Normal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            float Nsd = section.combinations[selectedEffort].Normal;

            ImGui::Text("O esforço normal solicitante está fora do intervalo resistente da seção.");
            ImGui::Separator();
            ImGui::Text("Nsd: %.2f", Nsd);
            ImGui::Text("Intervalo permitido:");
            ImGui::BulletText("Máx. Compressão: %.2f kN", section.internalForces.getMaxNormalCompression());
            ImGui::BulletText("Máx. Tração: %.2f kN", section.internalForces.getMaxNormalTraction());

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showPopUpErrorAxialForce = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    if (showPopUpErrorPolygon == true)
    {
        ImGuiIO &io = ImGui::GetIO();

        ImGui::OpenPopup("Erro de inserção de dados");

        // Define a posição para o centro da tela
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("O polígono não foi definido corretamente.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showPopUpErrorPolygon = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    if (showPopUpErrorBar == true)
    {
        ImGuiIO &io = ImGui::GetIO();

        ImGui::OpenPopup("Erro de inserção de dados");

        // Define a posição para o centro da tela
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Erro de inserção de dados", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("A armadura não foi definida corretamente.");
            ImGui::Separator();

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f); // Centraliza o botão
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                showPopUpErrorBar = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

void Interface::crossSectionTable(Section &section)
{
    if (ImGui::BeginTable("Tabela", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("x (cm)");
        ImGui::TableSetupColumn("y (cm)");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < section.workingPolygon.getPolygonVertices().size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", static_cast<int>(i + 1));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f", section.workingPolygon.getPolygonVertices()[i].getX());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.3f", section.workingPolygon.getPolygonVertices()[i].getY());
        }
        ImGui::EndTable();
    }
}

void Interface::RightTablePos(const char *nome1, const char *nome2, float posY, Section &section)
{
    ImGuiIO &io = ImGui::GetIO();

    float larguraFixa = 300.0f;
    float alturaTotal = io.DisplaySize.y - posY;
    float alturaCadaJanela = alturaTotal / 2.0f;

    // --- Janela 1: Tabela de Pontos ---
    ImVec2 posJanela1 = ImVec2(io.DisplaySize.x - larguraFixa, posY);

    ImGui::SetNextWindowPos(posJanela1, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(larguraFixa, alturaCadaJanela), ImGuiCond_Always);
    ImGui::Begin(nome1, nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Tabela de Pontos:");
    crossSectionTable(section);

    ImGui::End();

    // --- Janela 2: Tabela de Esforços ---
    ImVec2 posJanela2 = ImVec2(io.DisplaySize.x - larguraFixa, posY + alturaCadaJanela);

    ImGui::SetNextWindowPos(posJanela2, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(larguraFixa, alturaCadaJanela), ImGuiCond_Always);
    ImGui::Begin(nome2, nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Tabela de Esforços:");
    EffortsTable(section);

    ImGui::End();
}

void Interface::applyDarkElegantPlotStyle()
{
    ImPlotStyle &style = ImPlot::GetStyle();

    // Fundo transparente
    style.Colors[ImPlotCol_PlotBg] = ImVec4(0, 0, 0, 0.0f);
    style.Colors[ImPlotCol_FrameBg] = ImVec4(0, 0, 0, 0.0f);
    style.Colors[ImPlotCol_PlotBorder] = ImVec4(1, 1, 1, 0.05f);
    style.Colors[ImPlotCol_LegendBorder] = ImVec4(1, 1, 1, 0.05f); // redundante, mas ajuda

    // Eixos discretos (novos enums)
    style.Colors[ImPlotCol_AxisBg] = ImVec4(0, 0, 0, 0.1);    // fundo atrás do eixo
    style.Colors[ImPlotCol_AxisGrid] = ImVec4(1, 1, 1, 0.1);  // linhas de grade
    style.Colors[ImPlotCol_AxisTick] = ImVec4(1, 1, 1, 0.2);  // tracinhos
    style.Colors[ImPlotCol_AxisText] = ImVec4(1, 1, 1, 1.0f); // labels dos eixos

    // Tamanhos de ticks
    style.MajorTickLen = ImVec2(6, 6);
    style.MajorTickSize = ImVec2(1.0f, 1.0f);
}

void Interface::saveSectionData(Section &section, const std::string &filename)
{
    std::ofstream os(filename, std::ios::binary);

    if (!os.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << filename << '\n';
        return;
    }

    cereal::JSONOutputArchive archive(os);
    archive(CEREAL_NVP(section));
    std::cout << "Dados da seção salvos com sucesso em: " << filename << '\n';
}

// void Interface::loadSectionData(Section &section, const std::string &filename)
// {
//     std::ifstream is(filename, std::ios::binary);

//     if (!is.is_open())
//     {
//         std::cerr << "Erro ao abrir o arquivo para leitura: " << filename << '\n';
//         return;
//     }

//     cereal::JSONInputArchive archive(is);
//     archive(CEREAL_NVP(section));
//     std::cout << "Dados da secao carregados de: " << filename << '\n';

//     section.defineGeometry(section.originalPolygon, section.originalReinforcement);
//     section.defineMaterials(section.concrete, section.steel);
// }

void Interface::loadSectionData(Section &section, const std::string &filename)
{
    std::ifstream is(filename, std::ios::binary);

    if (!is.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << filename << '\n';
        // Aqui você pode adicionar um popup para o usuário se quiser informar que não conseguiu abrir o arquivo.
        // Por enquanto, apenas o erro no console já é um bom passo.
        return;
    }

    try
    {
        cereal::JSONInputArchive archive(is);
        archive(CEREAL_NVP(section));
        std::cout << "Dados da secao carregados de: " << filename << '\n';

        // Se o carregamento foi bem-sucedido, defina a geometria e materiais
        section.defineGeometry(section.originalPolygon, section.originalReinforcement);
        section.defineMaterials(section.concrete, section.steel);
        
        // Ative as flags de auto-ajuste para os gráficos
        shouldAutoFit = true;
        shouldAutoFitEnv = true;

        // Limpa o mapeamento de combinações, se houver
        if (!section.combinations.empty()) {
            for (size_t j = 0; j < mappingID.size(); ++j)
                mappingID[j] = false; 
        }
        
    }
    catch (const cereal::Exception &e)
    {
        // Captura exceções específicas do cereal
        std::cerr << "Erro ao carregar dados da secao (Cereal Exception): " << e.what() << '\n';
        // Ativa a flag para mostrar o popup de erro na interface
        showCorrupedFileError = true; 
    }
    catch (const std::exception &e)
    {
        // Captura outras exceções padrão (ex: problemas de E/S inesperados)
        std::cerr << "Erro inesperado ao carregar dados da secao: " << e.what() << '\n';
        showCorrupedFileError = true;
    }
    catch (...)
    {
        // Captura qualquer outra exceção não especificada
        std::cerr << "Erro desconhecido ao carregar dados da secao.\n";
        showCorrupedFileError = true;
    }
}

void Interface::autoFitToPointsWithMargin(const vector<Point> &points, float margin)
{
    if (points.size() < 2)
        return;

    double minX = points[0].getX();
    double maxX = points[0].getX();
    double minY = points[0].getY();
    double maxY = points[0].getY();

    for (const Point &p : points)
    {
        if (p.getX() < minX)
            minX = p.getX();
        if (p.getX() > maxX)
            maxX = p.getX();
        if (p.getY() < minY)
            minY = p.getY();
        if (p.getY() > maxY)
            maxY = p.getY();
    }

    double marginX = 0.1 * (maxX - minX);
    double marginY = 0.1 * (maxY - minY);

    if (marginX == 0)
        marginX = 1.0;
    if (marginY == 0)
        marginY = 1.0;

    ImPlot::SetupAxesLimits(
        minX - marginX, maxX + marginX,
        minY - marginY, maxY + marginY,
        ImGuiCond_Always);
}

void Interface::renderReinforcement(Section &section, std::string plotLabel)
{
    // Esta função DEVE ser chamada DENTRO de um ImPlot::BeginPlot()
    // para que ImPlot::PlotToPixels() funcione corretamente.

    // Obter a escala de pixels por unidade de dados (cm).
    // Isso deve ser feito APENAS UMA VEZ por frame, dentro do BeginPlot.
    ImPlotPoint p1_data = ImPlotPoint(0, 0);
    ImPlotPoint p2_data = ImPlotPoint(1.0, 0); // 1.0 cm de distância
    ImVec2 p1_pixels = ImPlot::PlotToPixels(p1_data);
    ImVec2 p2_pixels = ImPlot::PlotToPixels(p2_data);
    float pixels_per_cm_x = std::fabs(p2_pixels.x - p1_pixels.x);

    // Iterar sobre cada barra de armadura para desenhá-la individualmente
    for (size_t i = 0; i < section.getWorkingReinforcement().GetNumPoints(); ++i)
    {
        double x_bar, y_bar, diameter_bar_mm;
        // Obter as coordenadas e o diâmetro da barra.
        section.workingReinforcement.GetTableData(i, &x_bar, &y_bar, &diameter_bar_mm);

        // Converter o diâmetro de mm para cm (ou para a unidade do seu plot).
        float diameter_bar_cm = static_cast<float>(diameter_bar_mm / 10.0); // Diâmetro em cm

        // Calcular o tamanho do marcador em pixels para esta barra.
        float marker_size_pixels = (diameter_bar_cm * pixels_per_cm_x) / 2;

        // Garantir um tamanho mínimo para visualização, mesmo para diâmetros muito pequenos
        if (marker_size_pixels < 1.0f)
        { // Exemplo: tamanho mínimo de 2 pixels
            marker_size_pixels = 1.0f;
        }
// Configurar o estilo do marcador para a barra atual.
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle,
                                   marker_size_pixels,
                                   ImVec4(0.0f, 0.0f, 1.0f, 1.0f), // Cor azul
                                   1.0f,                           // Espessura da borda
                                   ImVec4(0.0f, 0.0f, 0.8f, 1.0f)  // Cor da borda
        );

        // Desenhar a barra individualmente.
        // É importante que o rótulo do PlotScatter seja único para cada barra
        // se você quiser interatividade individual (ex: tooltips).
        // Podemos usar um label_id com o índice.
        char bar_label[32];
        snprintf(bar_label, sizeof(bar_label), "%s Bar %zu", plotLabel.c_str(), i + 1);

        double single_x[] = {x_bar}; // PlotScatter espera um array, mesmo para um único ponto
        double single_y[] = {y_bar};

        ImPlot::PlotScatter(bar_label, single_x, single_y, 1);
    }
}