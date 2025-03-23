#include "Interface.h"
#include <functional>

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

void Interface::autorsWindow()
{

    ImGui::SetNextWindowPos(ImVec2(147, 20));
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
    ImGui::BulletText("Prof. Dr. Eduardo Vicente Wolf Trentini - etrentini@ufu.br ");
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
        ImGui::SetNextWindowPos(ImVec2(4, 35));
        ImGui::SetNextWindowSize(ImVec2(420, 270));
        ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove);

        // Input para número de pontos

        ImGui::Text("Vertices");
        ImGui::PushItemWidth(70);
        ImGui::BeginGroup();
        ImGui::InputDouble("x (cm)", &coordXPolygon, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("y (cm)", &coordYPolygon, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (ImGui::Button("Adicionar Ponto"))
        {
            section.polygon.addVertice(coordXPolygon, coordYPolygon);
        }

        ImGui::SameLine();

        if (ImGui::Button("Remover Ponto"))
        {
            if (!section.polygon.getPolygonVertices().empty())
                section.polygon.removeLastVertice();
        }

        ImGui::SameLine();

        if (ImGui::Button("Calcular parametros"))
        {
            section.polygon.computeArea();
            section.polygon.computeCentroid();
            section.polygon.computeMaxCoordY();
            section.polygon.computeMinCoordY();
            section.polygon.computeHeight();
        }

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

        crossSectionTable(section);

        ImGui::Text("Area: %.2f", section.polygon.getPolygonArea());
        ImGui::Text("MaxY: %.2f", section.polygon.getMaxY());
        ImGui::Text("MinY: %.2f", section.polygon.getMinY());
        ImGui::Text("Height: %.2f", section.polygon.getMaxY());
        ImGui::Text("CG: %.2f, %.2f", section.polygon.getGeometricCenter().getX(), section.polygon.getGeometricCenter().getY());
        ImGui::Text("Vet0: %.2f, %.2f", section.polygon.getVet0X(), section.polygon.getVet0Y());

        ImGui::End(); // Finaliza a janela
        ImGui::EndMenu();
    }
}

void Interface::interfaceMaterials(Section &section)
{
    if (ImGui::BeginMenu("Materiais"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(138, 35));                     // Posição inicial
        ImGui::Begin("Entrada de Dados de Materiais", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImVec2 plotSize = ImGui::GetContentRegionAvail();

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

            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::EndMenu();
    }
}

void Interface::concreteInterface(Section &section)
{
    static int constitutiveModel = 0;
    static double collectedFck, collectedGammaC, stress;

    ImGui::RadioButton("ABNT NBR 6118:2014", &constitutiveModel, 0);
    ImGui::SameLine();
    ImGui::RadioButton("ABNT NBR 6118:2023", &constitutiveModel, 1);

    if (constitutiveModel == 0)
    {
        StressStrainModelType model61182014 = StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2014;

        ImGui::Text("Parâmetros do Concreto");
        ImGui::PushItemWidth(70);
        ImGui::BeginGroup();
        ImGui::InputDouble("fck (MPa):", &collectedFck, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("gammaC: ", &collectedGammaC, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (collectedFck < 0 || collectedGammaC < 0)
        {
            collectedFck = 0;
            collectedGammaC = 1;
        }

        if (ImGui::Button("Adicionar"))
        {
            section.concrete.setParameters(model61182014, collectedFck, collectedGammaC);

            section.concrete.setCurveStressStrain();
        }

        ImVec2 plotSize = ImGui::GetContentRegionAvail();

        // inicialização do gráfico com os eixos
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto ABNT NBR 6118:2014", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxis(ImAxis_X1, " ε por mil");
            ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);

            renderStrainConcreteRuptureDiagram(section.concrete.getCurveStressStrain(), "TensaoxDef");
            // renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");

            double x_values[] = {0, section.concrete.getStrainConcretePlastic()};
            double y_values[] = {section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_values, y_values, 2);

            ImPlot::Annotation(0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), ImVec4(1, 0, 1, 0), ImVec2(0, -2.5), section.concrete.getStrainConcretePlastic(), " (β * fcd)  = %.2f MPa", section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()));
            double x_ec2[] = {section.concrete.getStrainConcretePlastic(), section.concrete.getStrainConcretePlastic()};
            double y_ec2[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ec2, y_ec2, 2);

            double x_ecu[] = {section.concrete.getStrainConcreteRupture(), section.concrete.getStrainConcreteRupture()};
            double y_ecu[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ecu, y_ecu, 2);

            ImPlot::Annotation(section.concrete.getStrainConcretePlastic(), 0, ImVec4(1, 0, 1, 0), ImVec2(-10, 0), section.concrete.getStrainConcretePlastic(), "%.2f", section.concrete.getStrainConcretePlastic());
            ImPlot::Annotation(section.concrete.getStrainConcreteRupture(), 0, ImVec4(1, 0, 1, 0), ImVec2(10, 0), section.concrete.getStrainConcreteRupture(), "%.2f", section.concrete.getStrainConcreteRupture());
        }

        ImPlot::EndPlot();
    }

    if (constitutiveModel == 1)
    {
        StressStrainModelType model61182023 = StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2023;

        ImGui::Text("Parâmetros do Concreto");
        ImGui::PushItemWidth(70);
        ImGui::BeginGroup();
        ImGui::InputDouble("fck (MPa):", &collectedFck, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("gammaC: ", &collectedGammaC, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (collectedFck < 0 || collectedGammaC < 0)
        {
            collectedFck = 0;
            collectedGammaC = 1;
        }

        if (ImGui::Button("Adicionar"))
        {
            section.concrete.setParameters(model61182023, collectedFck, collectedGammaC);

            section.concrete.setCurveStressStrain();
        }

        ImVec2 plotSize = ImGui::GetContentRegionAvail();

        // inicialização do gráfico com os eixos
        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto ABNT NBR 6118:2023", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxis(ImAxis_X1, " ε por mil");
            ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);

            renderStrainConcreteRuptureDiagram(section.concrete.getCurveStressStrain(), "TensaoxDef");
            // renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");

            double x_values[] = {0, section.concrete.getStrainConcretePlastic()};
            double y_values[] = {section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_values, y_values, 2);

            ImPlot::Annotation(0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()), ImVec4(1, 0, 1, 0), ImVec2(0, -2.5), section.concrete.getStrainConcretePlastic(), " (β *ηc * fcd)  = %.2f MPa", section.concrete.computeStress(-section.concrete.getStrainConcreteRupture()));
            double x_ec2[] = {section.concrete.getStrainConcretePlastic(), section.concrete.getStrainConcretePlastic()};
            double y_ec2[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ec2, y_ec2, 2);

            double x_ecu[] = {section.concrete.getStrainConcreteRupture(), section.concrete.getStrainConcreteRupture()};
            double y_ecu[] = {0, section.concrete.computeStress(-section.concrete.getStrainConcreteRupture())};
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImPlot::PlotLine("", x_ecu, y_ecu, 2);

            ImPlot::Annotation(section.concrete.getStrainConcretePlastic(), 0, ImVec4(1, 0, 1, 0), ImVec2(-10, 0), section.concrete.getStrainConcretePlastic(), "%.2f", section.concrete.getStrainConcretePlastic());
            ImPlot::Annotation(section.concrete.getStrainConcreteRupture(), 0, ImVec4(1, 0, 1, 0), ImVec2(10, 0), section.concrete.getStrainConcreteRupture(), "%.2f", section.concrete.getStrainConcreteRupture());
        }
        ImPlot::EndPlot();
    }
}

void Interface::steelInterface(Section &section)
{
    static double collectedFyk, collectedGammaS, collectedE, stress;
    ImGui::Text("Parâmetros da Armadura Passiva");
    ImGui::PushItemWidth(70);
    ImGui::BeginGroup();
    ImGui::InputDouble("fyk (MPa):", &collectedFyk, 0.0f, 0.0f, "%.3f");
    ImGui::InputDouble("gammaC: ", &collectedGammaS, 0.0f, 0.0f, "%.3f");
    ImGui::InputDouble("E (GPa):", &collectedE, 0.0f, 0.0f, "%.3f");
    ImGui::EndGroup();

    if (collectedFyk < 0 || collectedGammaS < 0 || collectedE < 0)
    {
        collectedFyk = 0;
        collectedGammaS = 1;
        collectedE = 1;
    }

    if (ImGui::Button("Adicionar"))
    {
        StressStrainSteelModelType modelPassive = StressStrainSteelModelType::PASSIVE_REINFORCEMENT;

        section.steel.setParameters(modelPassive, collectedFyk, collectedGammaS, collectedE);

        section.steel.setCurveStressStrain();
    }

    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    // inicialização do gráfico com os eixos
    if (ImPlot::BeginPlot("Diagrama Tensão-Deformação para aços de Armadura Passiva", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal | ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxis(ImAxis_X1, " ε por mil");
        ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");
        ImPlot::SetupAxesLimits((-section.steel.getStrainSteelRupture() * 1.1), (section.steel.getStrainSteelRupture() * 1.1),
                                (-section.steel.getFyd() * 1.5), (section.steel.getFyd() * 1.5), ImGuiCond_Always);
        renderStrainSteelDiagram(section.steel.getCurveStressStrain(), "TensaoxDef");

        ImPlot::Annotation(-section.steel.getStrainSteelYield(), section.steel.computeStress(-section.steel.getStrainSteelYield()), ImVec4(1, 1, 1, 0), ImVec2(-10, -2), section.steel.computeStress(-section.steel.getStrainSteelYield()), "fyd = %.2f MPa", section.steel.computeStress(-section.steel.getStrainSteelYield()));
        ImPlot::Annotation(section.steel.getStrainSteelYield(), section.steel.computeStress(section.steel.getStrainSteelYield()), ImVec4(1, 1, 1, 0), ImVec2(10, 2), section.steel.computeStress(section.steel.getStrainSteelYield()), "fyd = %.2f MPa", section.steel.computeStress(section.steel.getStrainSteelYield()));

        ImPlot::Annotation(-section.steel.getStrainSteelYield(), 0, ImVec4(1, 1, 1, 0), ImVec2(-2, -1), -section.steel.getStrainSteelYield(), "%.2f", -section.steel.getStrainSteelYield());
        ImPlot::Annotation(section.steel.getStrainSteelYield(), 0, ImVec4(1, 1, 1, 0), ImVec2(2, 1), section.steel.getStrainSteelYield(), "%.2f", section.steel.getStrainSteelYield());

        double x_eyd[] = {-section.steel.getStrainSteelYield(), -section.steel.getStrainSteelYield()};
        double y_eyd[] = {0, -section.steel.computeStress(section.steel.getStrainSteelYield())};
        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
        ImPlot::PlotLine("", x_eyd, y_eyd, 2);

        double x_eyd2[] = {section.steel.getStrainSteelYield(), section.steel.getStrainSteelYield()};
        double y_eyd2[] = {0, section.steel.computeStress(section.steel.getStrainSteelYield())};
        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
        ImPlot::PlotLine("", x_eyd2, y_eyd2, 2);

        // ImPlot::Annotation(reforco.epsilon_yd, tensaoY[3], ImVec4(1, 1, 1, 0), ImVec2(annotation_offset_x, 1), tensaoY[3], "fyd = %.2f MPa", tensaoY[3]);
    }

    ImPlot::EndPlot();
}

void Interface::reinforcementInterface(Section &section)
{
    if (ImGui::BeginMenu("Armadura"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(217, 35));                     // Posição inicial
        static int barMode, numBar = 0;
        static double coordXBar, coordYBar, diameterBar, coordXiBar, coordXfBar, coordYiBar, coordYfBar;

        ImGui::Begin("Entrada de dados: Armadura Passiva", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::RadioButton("Uma barra", &barMode, 0);
        ImGui::RadioButton("Linha de barras", &barMode, 1);

        if (barMode == 0)
        {
            ImGui::PushItemWidth(70);
            ImGui::BeginGroup();
            ImGui::InputDouble("Diametro da barra (mm)", &diameterBar, 0.0f, 0.0f, "%.3f");
            ImGui::InputDouble("x (cm)", &coordXBar, 0.0f, 0.0f, "%.3f");
            ImGui::InputDouble("y (cm)", &coordYBar, 0.0f, 0.0f, "%.3f");
            ImGui::EndGroup();

            if (diameterBar < 0)
                diameterBar = 0;

            if (ImGui::Button("Adicionar"))
                section.reinforcement.addReinforcement(coordXBar, coordYBar, diameterBar);

            ImGui::SameLine();

            if (ImGui::Button("Remover"))
            {
                if (!section.reinforcement.getReinforcement().empty())
                    section.reinforcement.removeLastBar();
            }

            ImGui::SameLine();
            if (ImGui::Button("Limpar"))
            {
                section.reinforcement.clearReinforcement();
            }
        }

        if (barMode == 1)
        {
            ImGui::SetNextItemWidth(100);
            ImGui::InputInt("Numero de barras na linha", &numBar);
            ImGui::PushItemWidth(50);
            ImGui::BeginGroup();
            ImGui::InputDouble("Diametro das barras (mm)", &diameterBar, 0.0f, 0.0f, "%.3f");

            ImGui::InputDouble("xi (cm)", &coordXiBar, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::InputDouble("yi (cm)", &coordYiBar, 0.0f, 0.0f, "%.3f");
            ImGui::InputDouble("xf (cm)", &coordXfBar, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::InputDouble("yf (cm)", &coordYfBar, 0.0f, 0.0f, "%.3f");
            ImGui::EndGroup();

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

                    section.reinforcement.addReinforcement(coordX, coordY, diameterBar);
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Remover"))
            {
                if (!section.reinforcement.getReinforcement().empty())
                    section.reinforcement.removeLastBar();
            }

            ImGui::SameLine();
            if (ImGui::Button("Limpar"))
            {
                section.reinforcement.clearReinforcement();
            }
        }

        if (ImGui::BeginTable("Tabela", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("x (cm)");
            ImGui::TableSetupColumn("y (cm)");
            ImGui::TableSetupColumn("Diâmetro (mm)");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < section.reinforcement.getReinforcement().size(); ++i)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", static_cast<int>(i + 1));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", section.reinforcement.getReinforcement()[i].getX());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", section.reinforcement.getReinforcement()[i].getY());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", section.reinforcement.getDiameters()[i]);
            }
            ImGui::EndTable();
        }

        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::effortSectionInterface(Section &section)
{
    if (ImGui::BeginMenu("Esforços"))
    {
        ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always); // Ajuste os valores conforme necessário
        ImGui::SetNextWindowPos(ImVec2(289, 35));                     // Posição inicial
        static double N, Mx, My;

        ImGui::Begin("Entrada de Dados: Esforços", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(50);
        ImGui::BeginGroup();
        ImGui::InputDouble("N (kN)", &N, 0.0f, 0.0f, "%.3f");
        ImGui::InputDouble("Mx (kN.m)", &Mx, 0.0f, 0.0f, "%.3f");
        // ImGui::InputDouble("My (kN.m)", &My, 0.0f, 0.0f, "%.3f");
        ImGui::EndGroup();

        if (ImGui::Button("Adicionar"))
        {
            section.combinations.emplace_back(N, Mx, My);
        }

        ImGui::SameLine();
        if (ImGui::Button("Remover"))
        {

            if (section.combinations.size() > 0)
                section.combinations.pop_back();
        }

        ImGui::SameLine();
        if (ImGui::Button("Limpar"))
        {
            if (section.combinations.size() > 0)
                section.combinations.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Calcular"))
        {
        }

        EffortsTable(section);

        ImGui::End();
        ImGui::EndMenu();
    }
}

void Interface::crossSectionPlotInterface(Section &section)
{
    ImGui::Begin("Grafico da Secao Transversal");

    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    // inicialização do gráfico com os eixos
    if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
    {
        if (section.polygon.getPolygonVertices().size() > 2)
        {
            renderPolygon(section.polygon.getPolygonVertices(), "Vertices", "Polygon");
            renderVectorPoint(section.reinforcement.getReinforcement(), "Barras");
        }
    }

    ImPlot::EndPlot();

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

        // ImPlot::SetNextLineStyle(ImVec4(0.53f, 0.81f, 0.98f, 1.0f));

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.53f, 0.81f, 0.98, 1.0f));

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f); // mais grossa
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

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.53f, 0.81f, 0.98, 1.0f));

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f); // mais grossa
        ImPlot::PlotLine(nameVectorPoint.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vectorPoint.size()));

        ImPlot::PopStyleVar();
        ImPlot::PopStyleColor();
    }
}

void Interface::EffortsTable(Section &section)
{
    if (ImGui::BeginTable("Tabela", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Normal (kN)");
        ImGui::TableSetupColumn("Momento em X (kN.m)");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < section.combinations.size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", static_cast<int>(i + 1));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f", section.combinations[i].Normal);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.3f", section.combinations[i].MsdX);
            // ImGui::TableSetColumnIndex(3);
            // ImGui::Text("%.3f", section.reinforcement.getDiameters()[i]);
        }
        ImGui::EndTable();
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