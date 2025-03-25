#include "Interface.h"

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
        ImGui::SetNextItemWidth(80);
        ImGui::Spacing(); // Adiciona um espaçamento antes da tabela
        ImGui::Text("Vertices");
        ImGui::InputDouble("x (cm)", &coordXPolygon);
        ImGui::InputDouble("y (cm)", &coordYPolygon);

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

        ImGui::SameLine();

        if (ImGui::Button("SecaoT"))
        {
            vector<Point> collectedPoints = {{7.5, 0}, {10, 30}, {20, 40}, {20, 50}, {-20, 50}, {-20, 40}, {-10, 30}, {-7.5, 0}};
            vector<Point> collectedReinforcement = {
                {5, 2.5},
                {5, 7.5},
                {-5, 7.5},
                {-5, 2.5},
            };
            vector<double> collectedDiameters = {10, 10, 10, 10};
            section.polygon.setVertices(collectedPoints);
            section.reinforcement.setReinforcement(collectedReinforcement, collectedDiameters);
        }

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
        ImGui::SetNextWindowPos(ImVec2(123, 47));                     // Posição inicial
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
    ImGui::RadioButton("ABNT NBR 6118:2023", &constitutiveModel, 1);

    if (constitutiveModel == 0)
    {
        StressStrainConcreteModelType model61182014 = StressStrainConcreteModelType::PARABOLA_RECTANGLE_NBR6118_2014;
        ImGui::InputDouble("fck (MPa):", &collectedFck);
        ImGui::InputDouble("gammaC: ", &collectedGammaC);

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
        if (ImPlot::BeginPlot("Concreto", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
        {
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);
            renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");
        }

        ImPlot::EndPlot();
    }

    if (constitutiveModel == 1)
    {
        StressStrainConcreteModelType model61182023 = StressStrainConcreteModelType::PARABOLA_RECTANGLE_NBR6118_2023;
        ImGui::InputDouble("fck (MPa):", &collectedFck);
        ImGui::InputDouble("gammaC: ", &collectedGammaC);

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
        if (ImPlot::BeginPlot("Concreto", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
        {
            ImPlot::SetupAxesLimits(0, (section.concrete.getStrainConcreteRupture() * 1.1), 0, section.concrete.getFcd(), ImGuiCond_Always);
            renderVectorPoint(section.concrete.getCurveStressStrain(), "TensaoxDef");
        }

        ImPlot::EndPlot();
    }
}

void Interface::steelInterface(Section &section)
{
    static double collectedFyk, collectedGammaS, collectedE, stress;

    ImGui::InputDouble("fyk (MPa):", &collectedFyk);
    ImGui::InputDouble("gammaC: ", &collectedGammaS);
    ImGui::InputDouble("E (GPa)", &collectedE);

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
    if (ImPlot::BeginPlot("Aço", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
    {
        ImPlot::SetupAxesLimits((-section.steel.getStrainSteelRupture() * 1.1), (section.steel.getStrainSteelRupture() * 1.1),
                                (-section.steel.getFyd() * 1.1), (section.steel.getFyd() * 1.1), ImGuiCond_Always);
        renderVectorPoint(section.steel.getCurveStressStrain(), "TensaoxDef");
    }

    ImPlot::EndPlot();
}

void Interface::reinforcementInterface(Section &section)
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
                section.reinforcement.addReinforcement(coordXBar, coordYBar, diameterBar);

            ImGui::SameLine();

            if (ImGui::Button("Remover"))
            {
                if (!section.reinforcement.getReinforcement().empty())
                    section.reinforcement.removeLastReinforcement();
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

                    section.reinforcement.addReinforcement(coordX, coordY, diameterBar);
                }

                section.reinforcement.computeArea();
            }

            ImGui::SameLine();

            if (ImGui::Button("Remover"))
            {
                if (!section.reinforcement.getReinforcement().empty())
                    section.reinforcement.removeLastReinforcement();
            }
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
        ImGui::SetNextWindowPos(ImVec2(200, 35));                     // Posição inicial
        static double Nsd, Mx, My, eps1, eps2;
        static bool showPopUpErrorAxialForce = false;
        static bool showPopUpSolver = false;

        ImGui::Begin("Entrada de Dados: Esforços", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::InputDouble("N (kN)", &Nsd);
        ImGui::InputDouble("Mx (kN.m)", &Mx);
        ImGui::InputDouble("My (kN.m)", &My);
        ImGui::InputDouble("eps1 (mm/m)", &eps1);
        ImGui::InputDouble("eps2 (mm/m)", &eps2);

        if (ImGui::Button("Comparar"))
        {
            section.setSectionProperties(section.polygon, section.reinforcement, section.concrete, section.steel, NormativeIntegrationVersion::ABNT_NBR6118_2014);
            section.setStrainDistribution(eps1, eps2);
            section.setStressRegions();
            section.computeInternalForces(Nsd);
            section.printSectionData();
        }

        if (ImGui::Button("Calcular"))
        {
            section.setSectionProperties(section.polygon, section.reinforcement, section.concrete, section.steel, NormativeIntegrationVersion::ABNT_NBR6118_2014);
            section.internalForces.setNormalSolicitation(Nsd);
            section.internalForces.computeMaxCompression(section.polygon, section.reinforcement, section.steel, section.concrete);
            section.internalForces.computeMaxTraction(section.polygon, section.reinforcement, section.steel);

            if (section.internalForces.getNormalSolicitation() < section.internalForces.getMaxNormalCompression() ||
                section.internalForces.getNormalSolicitation() > section.internalForces.getMaxNormalTraction())
            {
                showPopUpErrorAxialForce = true;
                ImGui::OpenPopup("Erro de Esforço Normal");
            }
            else
            {
                section.computeSectionEquilibriumSolver(Nsd);
                showPopUpSolver = true;
                ImGui::OpenPopup("Calculo do Momento Resistente");
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
            renderPolygon(section.stressRegions.getCompressedRegion().getPolygonVertices(), "vComp", "pComp");
            renderPolygon(section.stressRegions.getParabolicRegion().getPolygonVertices(), "vParab", "pParab");
            renderPolygon(section.stressRegions.getRectangularRegion().getPolygonVertices(), "vRec", "pRec");
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