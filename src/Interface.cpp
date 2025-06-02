#include "Interface.h"
#include "segoeuisl_data.h"

#include <iostream>
#include <functional>
#include <algorithm>


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
    //ImFont *customFont = io.Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);

    ImFont *customFont = io.Fonts->AddFontFromMemoryTTF(
        (void*) segoeuisl_ttf,
        segoeuisl_ttf_len,
        18.0f,
        &fontConfig,
        customRange
    );

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
            if(ImGui::MenuItem("Salvar"))
            {
                saveSectionData(section, "projeto.json");
            }

            if(ImGui::MenuItem("Carregar"))
            {
                loadSectionData(section, "projeto.json");
            }
            
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
    if (ImGui::BeginMenu("Seção Transversal")) // Primeira versão, não é a final - precisa incrementar vértices temporarios - não adicionar vertices negativos e tal
    {
        static double coordXPolygon, coordYPolygon;
        static bool showPopUpErrorPolygon = false;
        static int tempNumPoints = 0;

        if (tempNumPoints < 0)
            tempNumPoints = 0;

        ImGui::SetNextWindowPos(ImVec2(3, 47));
        ImGui::SetNextWindowSize(ImVec2(420, 270));
        ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        ImGui::SeparatorText("Quantidade de pontos do polígono:");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##xx", &tempNumPoints); // Input para número de pontos
        ImGui::SeparatorText("Seções transversais pré-definidas:");

        if (ImGui::Button("Seção T"))
        {
            section.polygon.clearPolygonVertices();

            vector<Point> collectedPoints = {
                {7.5, 0}, {10, 30}, {20, 40}, {20, 50}, {-20, 50}, {-20, 40}, {-10, 30}, {-7.5, 0}};

            vector<Point> collectedReinforcement = {
                {5, 2.5}, {5, 7.5}, {-5, 7.5}, {-5, 2.5}};

            vector<double> collectedDiameters = {10, 10, 10, 10};

            tempNumPoints = collectedPoints.size();
            section.polygon.setVertices(collectedPoints);

            section.reinforcement.setReinforcement(collectedReinforcement, collectedDiameters);
            section.reinforcement.computeArea(); // não esquece isso!
        }

        // Input para número de pontos

        if (tempNumPoints != section.polygon.GetNumPoints())
        {
            section.polygon.SetNumPoints(tempNumPoints); // Ajusta o número de pontos na classe Polygon
        }

        if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            // Configura as colunas com larguras fixas e centralizadas
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("x (cm)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("y (cm)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableHeadersRow();

            // Itera sobre todos os pontos do polígono
            for (int row = 0; row < section.polygon.GetNumPoints(); row++)
            {
                // Usando PushID para garantir ID único para cada linha
                ImGui::PushID(row);

                ImGui::TableNextRow();

                // Exibe o índice do ponto
                ImGui::TableSetColumnIndex(0); // Coluna para 'ID'
                ImGui::Text("%d", row + 1);    // Exibe o índice do ponto (começa de 1)

                // Exibe a coordenada X
                ImGui::TableSetColumnIndex(1); // Coluna para 'x'
                char labelX[10];
                snprintf(labelX, sizeof(labelX), "##xx", row); // Cria o label para cada coordenada X
                float x, y;
                section.polygon.GetTableData(row, &x, &y); // Obter as coordenadas do ponto na linha 'row'

                // Cria um campo editável para a coordenada x
                if (ImGui::InputFloat(labelX, &x))
                {
                    // Se o valor de x for alterado, atualiza o ponto correspondente
                    section.polygon.SetTableData(row, x, y); // Atualiza a coordenada 'x' diretamente no vetor
                }

                // Exibe a coordenada Y
                ImGui::TableSetColumnIndex(2); // Coluna para 'y'
                char labelY[10];
                snprintf(labelY, sizeof(labelY), "##yy", row); // Cria o label para cada coordenada Y

                // Cria um campo editável para a coordenada y
                if (ImGui::InputFloat(labelY, &y))
                {
                    // Se o valor de y for alterado, atualiza o ponto correspondente
                    section.polygon.SetTableData(row, x, y); // Atualiza a coordenada 'y' diretamente no vetor
                }

                ImGui::PopID(); // Remove o ID do ponto atual após a linha ter sido processada
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Limpar"))
        {
            section.polygon.clearPolygonVertices();
            tempNumPoints = 0;
        }

        ImGui::SameLine();

        if (ImGui::Button("Calcular parametros"))
        {
            if (!section.polygon.getPolygonVertices().empty())
            {
                section.polygon.computeArea();
                section.polygon.computeCentroid();
                section.polygon.computeMaxCoordY();
                section.polygon.computeMinCoordY();
                section.polygon.computeHeight();
            }
            else
            {
                showPopUpErrorPolygon = true;
                ImGui::OpenPopup("Vértices vazios");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Transladar"))
        {
            section.polygon.translateToCentroid();
            section.reinforcement.translateToCentroidPolygon(section.polygon.getGeometricCenter());
        }

        ImGui::SameLine();

        if (ImGui::Button("Rotacionar"))
        {
            section.polygon.rotateAroundCentroid(10);
            section.reinforcement.rotateAroundCentroidPolygon(10, section.polygon.getGeometricCenter());
        }

        ImGui::Text("Area: %.2f |", section.polygon.getPolygonArea());
        ImGui::SameLine();
        ImGui::Text("MaxY: %.2f |", section.polygon.getMaxY());
        ImGui::SameLine();
        ImGui::Text("MinY: %.2f |", section.polygon.getMinY());
        ImGui::SameLine();
        ImGui::Text("Height: %.2f |", section.polygon.getMaxY());
        ImGui::SameLine();
        ImGui::Text("CG: %.2f, %.2f |", section.polygon.getGeometricCenter().getX(), section.polygon.getGeometricCenter().getY());
        ImGui::Text("Vet0: %.2f, %.2f |", section.polygon.getVet0X(), section.polygon.getVet0Y());

        if (showPopUpErrorPolygon)
        {
            if (ImGui::BeginPopupModal("Vértices vazios", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Adicione os vértices da seção e depois calcule os parâmetros");

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpErrorPolygon = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End(); // Finaliza a janela
        ImGui::EndMenu();
    }
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
    static int constitutiveModel = 0;
    static double collectedFck = 30, collectedGammaC = 1.4, stress;
    int x, y;

    if (collectedFck < 0 || collectedGammaC < 0)
    {
        collectedFck = 35;
        collectedGammaC = 1.4;
    }

    if (collectedFck > 90)
    {
        collectedFck = 90;
    }

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

        section.concrete.setParameters(model61182014, collectedFck, collectedGammaC);
        section.concrete.setCurveStressStrain();

        ImVec2 plotSize = ImGui::GetContentRegionAvail();
        ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual
        applyDarkElegantPlotStyle();
        // inicialização do gráfico com os eixos
        ImGui::SetCursorPos(ImVec2(0, 70)); // Define a posição do cursor
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto NBR 6118:2014", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
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

        section.concrete.setParameters(model61182023, collectedFck, collectedGammaC);
        section.concrete.setCurveStressStrain();

        ImPlotStyle backup = ImPlot::GetStyle(); // salva estilo atual

        applyDarkElegantPlotStyle();
        ImVec2 plotSize = ImGui::GetContentRegionAvail();

        ImGui::SetCursorPos(ImVec2(0, 70)); // Define a posição do cursor
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto NBR 6118:2023", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
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
    static double collectedFyk = 500, collectedGammaS = 1.15, collectedE = 210, stress;

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
    if (ImPlot::BeginPlot("Diagrama Tensão-Deformação AÇO NBR 6118:2023", ImVec2(630, 430), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxis(ImAxis_X1, " ε ‰ ");
        ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
        ImPlot::SetupAxesLimits((-section.steel.getStrainSteelRupture() * 1.1), (section.steel.getStrainSteelRupture() * 1.1),
                                (-section.steel.getFyd() * 1.5), (section.steel.getFyd() * 1.5), ImGuiCond_Always);
        renderStrainSteelDiagram(section.steel.getCurveStressStrain(), "TensaoxDef");

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
        static double coordXBar, coordYBar, diameterBar = 0;
        static double coordXiBar, coordXfBar, coordYiBar, coordYfBar;
        static bool showPopUpErrorBar = false;

        if (section.reinforcement.getReinforcement().empty())
            tempNumPoints = 0; // Se o vetor de armadura estiver vazio, o número de pontos temporário é 0
        else
            tempNumPoints = section.reinforcement.getReinforcement().size(); // Atualiza o número de pontos temporário com o tamanho do vetor de armadura

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

            if (tempNumPoints != section.reinforcement.getReinforcement().size())
                section.reinforcement.SetNumPoints(tempNumPoints);

            ImGui::SameLine();
            if (ImGui::Button("Limpar Tudo"))
                section.reinforcement.clearReinforcement();
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

            if (tempNumPoints != section.reinforcement.getReinforcement().size())
                section.reinforcement.SetNumPoints(tempNumPoints);

            ImGui::SameLine();
            if (ImGui::Button("Limpar Tudo"))
                section.reinforcement.clearReinforcement();
            ImGui::PopID(); // Remove o ID do ponto atual após a linha ter sido processada

            ImGui::SeparatorText("Linha de Barras:");
            ImGui::PushID(2); // Garante que o ID seja único para cada barra
            ImGui::InputInt("Número de barras", &numBar);
            if (numBar < 2)
                numBar = 2;
            ImGui::InputDouble("xi (cm)", &coordXiBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::InputDouble("xf (cm)", &coordXfBar, 0.0, 0.0, "%.2f");
            ImGui::InputDouble("yi (cm)", &coordYiBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::InputDouble("yf (cm)", &coordYfBar, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            ImGui::InputDouble("Diâmetro (mm)", &diameterBar, 0.0, 0.0, "%.2f");
            ImGui::PopID();
            ImGui::EndGroup();

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
                        section.reinforcement.addReinforcement(x, y, diameterBar);
                    }
                    section.reinforcement.computeArea();
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

            for (size_t i = 0; i < section.reinforcement.GetNumPoints(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", static_cast<int>(i + 1)); // ID da barra

                ImGui::TableSetColumnIndex(1);
                char labelX[10];
                snprintf(labelX, sizeof(labelX), "##x%d", i); // Cria o label para cada coordenada X
                double x, y, d;
                section.reinforcement.GetTableData(i, &x, &y, &d); // Obter as coordenadas do ponto na linha 'row'

                if (ImGui::InputDouble(labelX, &x, 0.0, 0.0, "%.2f")) // Cria um campo editável para a coordenada x
                {
                    section.reinforcement.SetTableData(i, x, y, d); // Atualiza a coordenada 'x' diretamente no vetor
                }
                ImGui::TableSetColumnIndex(2); // Coluna para 'y'
                char labelY[10];
                snprintf(labelY, sizeof(labelY), "##y%d", i); // Cria o label para cada coordenada Y

                if (ImGui::InputDouble(labelY, &y, 0.0, 0.0, "%.2f")) // Cria um campo editável para a coordenada y
                {
                    section.reinforcement.SetTableData(i, x, y, d); // Atualiza a coordenada 'y' diretamente no vetor
                }

                ImGui::TableSetColumnIndex(3); // Coluna para 'Diâmetro'
                char labelD[10];
                snprintf(labelD, sizeof(labelD), "##d%d", i);         // Cria o label para cada diâmetro
                if (ImGui::InputDouble(labelD, &d, 0.0, 0.0, "%.2f")) // Cria um campo editável para o diâmetro
                {
                    if (d > 0)
                    {
                        section.reinforcement.SetTableData(i, x, y, d); // Atualiza o diâmetro diretamente no vetor
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

    ImGui::SeparatorText("Modos de Ruptura Comuns");

    ImGui::BulletText("Flexão: falha por tração no aço ou compressão no concreto");
    ImGui::BulletText("Corte: ruptura por cisalhamento, controlado com estribos");
    ImGui::BulletText("Punção: comum em lajes lisas sem vigas");

    ImGui::Separator();
    ImGui::TextWrapped("Nota: Os valores e fórmulas seguem as recomendações da NBR 6118:2023, podendo variar conforme o tipo estrutural e critérios de segurança do projeto.");
}

void Interface::effortSectionInterface(Section &section)
{
    if (ImGui::BeginMenu("Esforços"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(265, 47));                     // Posição inicial
        static double Nsd, Mx, My, eps1, eps2;
        static bool showPopUpErrorAxialForce = false;
        static bool showPopUpSolver = false;
        static int tempNumPoints = 0;

        ImGui::Begin("Entrada de Dados: Esforços", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::PushItemWidth(100);
        ImGui::SeparatorText("Número de combinações de esforços");
        ImGui::InputInt("##XX:", &tempNumPoints);

        if (tempNumPoints < 0)
            tempNumPoints = 0;

        if (tempNumPoints != section.combinations.size())
        {
            section.combinations.resize(tempNumPoints, Combination(0.0f, 0.0f, 0.0f, 0.0f));
        }

        if (ImGui::BeginTable("TabelaEsforcos", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Nsd (kN)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("M,x (kN.m)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            /* -- Versão 1.0 Momento em apenas uma direção --
            ImGui::TableSetupColumn("My (kN.m)", ImGuiTableColumnFlags_WidthFixed, 120.0f); */
            ImGui::TableHeadersRow();

            for (int i = 0; i < section.combinations.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();

                float nsd = section.combinations[i].Normal;
                float mx = section.combinations[i].MsdX;
                // float my = section.combinations[i].MsdY;

                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);

                // Nsd
                ImGui::TableSetColumnIndex(1);
                char labelN[16];
                snprintf(labelN, sizeof(labelN), "##nsd%d", i);
                if (ImGui::InputFloat(labelN, &nsd))
                    section.combinations[i].Normal = nsd;

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Acrescente sinal Positivo para tração e Negativo para compressão");
                }

                // Mx
                ImGui::TableSetColumnIndex(2);
                char labelMx[16];
                snprintf(labelMx, sizeof(labelMx), "##mx%d", i);
                if (ImGui::InputFloat(labelMx, &mx))
                    section.combinations[i].MsdX = mx;

                /* My -- Versão 1.0 Momento em apenas uma direção
                ImGui::TableSetColumnIndex(3);
                char labelMy[16];
                snprintf(labelMy, sizeof(labelMy), "##my%d", i);
                if (ImGui::InputFloat(labelMy, &my))
                    section.combinations[i].MsdY = my; */

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Limpar"))
        {
            if (section.combinations.size() > 0)
                section.combinations.clear();
            tempNumPoints = 0;
        }

        ImGui::SameLine();

        if (ImGui::Button("Calcular"))
        {
            section.setSectionProperties(section.polygon, section.reinforcement, section.concrete, section.steel, NormativeIntegrationVersion::ABNT_NBR6118_2014);
            section.internalForces.computeMaxCompression(section.polygon, section.reinforcement, section.steel, section.concrete);
            section.internalForces.computeMaxTraction(section.polygon, section.reinforcement, section.steel);

            for (int i = 0; i < section.combinations.size(); ++i)
            {
                section.internalForces.setNormalSolicitation(section.combinations[i].Normal);

                if (section.internalForces.getNormalSolicitation() < section.internalForces.getMaxNormalCompression() || section.internalForces.getNormalSolicitation() > section.internalForces.getMaxNormalTraction())
                    section.combinations[i].isMomentValid = false;

                else
                {
                    section.computeSectionEquilibriumSolver(section.combinations[i].Normal);
                    section.combinations[i].Msolver = section.momentSolver.getMomentCapacity();
                    section.combinations[i].isMomentValid = true;
                }
            }
        }

        ImGui::SeparatorText("Debug");

        if (ImGui::Button("Comparar"))
        {
            section.setSectionProperties(section.polygon, section.reinforcement, section.concrete, section.steel, NormativeIntegrationVersion::ABNT_NBR6118_2014);
            section.setStrainDistribution(eps1, eps2);
            section.setStressRegions();
            section.computeInternalForces(Nsd);
            section.printSectionData();
        }

        ImGui::PushItemWidth(50);
        ImGui::BeginGroup();
        ImGui::InputDouble("eps1: (mm/m)", &eps1, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("eps2: (mm/m)", &eps2, 0.0f, 0.0f, "%.3f");
        // ImGui::InputDouble("My (kN.m)", &My, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

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

        if (showPopUpSolver)
        {
            if (ImGui::BeginPopupModal("Calculo do Momento Resistente", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Momento Resistente: %.4f", section.momentSolver.getMomentCapacity());
                ImGui::Separator();
                ImGui::Text("eps1: %.4f", section.momentSolver.getTopFiberStrain());
                ImGui::Text("eps2: %.4f", section.momentSolver.getBottomFiberStrain());
                ImGui::Separator();
                ImGui::Text("iteracoes: %d", section.momentSolver.getIterations());

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    showPopUpSolver = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::crossSectionPlotInterface(Section &section, float posY)
{
    ImGuiIO &io = ImGui::GetIO();

    float largura = io.DisplaySize.x - 300.0f; // considerando a janela da direita com 300px
    float altura = io.DisplaySize.y - posY;    // altura que sobra a partir de posY

    ImGui::SetNextWindowPos(ImVec2(0, posY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(largura, altura), ImGuiCond_Always);

    ImGui::Begin("Grafico da Secao Transversal",
                 nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("Gráfico da Seção Transversal", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal | ImPlotAxisFlags_AutoFit))
    {
        if (section.polygon.getPolygonVertices().size() > 2)
        {

            renderPolygon(section.polygon.getPolygonVertices(), "Vertices", "Polygon");
            renderPolygon(section.stressRegions.getCompressedRegion().getPolygonVertices(), "vComp", "pComp");
            renderPolygon(section.stressRegions.getParabolicRegion().getPolygonVertices(), "vParab", "pParab");
            renderPolygon(section.stressRegions.getRectangularRegion().getPolygonVertices(), "vRec", "pRec");
            renderVectorPoint(section.reinforcement.getReinforcement(), "Barras");
        }

        ImPlot::EndPlot();
    }

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

        ImPlot::PlotScatter(nameVertices.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(polygonVertices.size()));

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

    if (ImGui::BeginTable("Tabela", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Nsd (kN)", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("M,x (kN.m)", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("M. Res. (kN.m)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
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

                section.computeSectionEquilibriumSolver(section.combinations[i].Normal);

                if (section.combinations[i].isMomentValid)
                    showPopUpSolver = true;
                else
                    showPopUpErrorAxialForce = true;
            }
            ImGui::SameLine();
            ImGui::Text("%d", static_cast<int>(i + 1));

            // Coluna 1 - Nsd
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f", section.combinations[i].Normal);

            // Coluna 2 - MsdX
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.2f", section.combinations[i].MsdX);

            // Coluna 3 - M. Resistente (com destaque em vermelho se inválido)
            ImGui::TableSetColumnIndex(3);
            if (!section.combinations[i].isMomentValid)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::TextUnformatted("ERRO!");
                ImGui::PopStyleColor();
            }

            else
            {
                ImGui::Text("%.2f", section.combinations[i].Msolver);
            }
        }

        ImGui::EndTable();
    }

    // --- POPUP: Momento resistente calculado com sucesso
    if (showPopUpSolver && selectedEffort >= 0)
    {
        ImGui::OpenPopup("Calculo do Momento Resistente");
        if (ImGui::BeginPopupModal("Calculo do Momento Resistente", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Momento Resistente: %.4f", section.momentSolver.getMomentCapacity());
            ImGui::Separator();
            ImGui::Text("eps1: %.4f", section.momentSolver.getTopFiberStrain());
            ImGui::Text("eps2: %.4f", section.momentSolver.getBottomFiberStrain());
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
        ImGui::OpenPopup("Erro de Esforço Normal");
        if (ImGui::BeginPopupModal("Erro de Esforço Normal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            float Nsd = section.combinations[selectedEffort].Normal;

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
}

void Interface::crossSectionTable(Section &section)
{
    if (ImGui::BeginTable("Tabela", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("x (cm)");
        ImGui::TableSetupColumn("y (cm)");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < section.polygon.getPolygonVertices().size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", static_cast<int>(i + 1));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f", section.polygon.getPolygonVertices()[i].getX());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.3f", section.polygon.getPolygonVertices()[i].getY());
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

void Interface::loadSectionData(Section &section, const std::string &filename)
{
    std::ifstream is(filename, std::ios::binary);

    if(!is.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << filename << '\n';
        return;
    }

    cereal::JSONInputArchive archive(is);
    archive(CEREAL_NVP(section));
    std::cout << "Dados da secao carregados de: " << filename << '\n';

    section.setPolygon(section.polygon);
    section.setReinforcement(section.reinforcement);
    section.setConcrete(section.concrete);
    section.setSteel(section.steel);
}