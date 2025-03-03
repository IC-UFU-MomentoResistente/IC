#include "raylib.h"
#include "imgui.h"
#include "../lib/rlImGui/rlImGui.h"
#include "implot.h"
#include "poligono.h"
#include "concreto.h"
#include <iostream>
#include <vector>
#include "reforco.h"
#include <string>
#include <fstream>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>

/*
- Tem que colocar unidade em tudo, tudo tem que ter a unidade, até os campos que o usuário digita. Eu prefiro que a unidade fique depoius do campo;
- Tem que ficar tudo padrão tbm. Por exemplo, tem lugar que o eixo x do gráfico ta escrito epsilon, e tem lugar que ta escrito deformação;
- tem que arrumar o desengho do grafico tbm, em vez de fazer de 0,1 em 0,1 a deformação, faz tipo, 60 intervalos;
- Todo campo tem que ter um valor máximo e mínimo de entrada. Se o usuario digita fora desse valor, vc volta e coloca dentro.
- Tem que dar uma passada geral e ir arrumando essas coisinhas.

- ID FIXO E REDIMENSIONAR A JANELA
- COLOCAR COMBINACAO
- MUDAR AS POSIÇÕES DO DIAGRAMA DE TENSAO DEFORMAÇÃO
- CONSOLE OUTPUT NORMAL - MOMENTO

*/

Reforco reforco;
std::vector<Point> collectedPoints = {{-10, -20}, {10, -20}, {10, 20}, {-10, 20}};

struct Combinacao
{
    float Normal;
    float MsdX;
    float MsdY;
};

Poligono poligono;
Poligono poligonoComprimido;
Poligono poligonoAreaParabola;
Poligono poligonoAreaRetangulo;

std::vector<Point> Rot = collectedPoints;
std::vector<Point> pontosOriginais = collectedPoints;

std::vector<float> EPIx = {-10.2, -10.0, -9, -8, -6, -4, -3, -2, -1, 0, 1, 2, 3, 4, 6, 8, 9, 10, 10.1, 10.2}; // DIAGRAMA DE DEFORMAÇÃO DO AÇO
std::vector<Combinacao> combinacao;
std::vector<Point> NormalxEPI2;                                                                                     // DIAGRAMA NORMAL X EPI2
std::vector<Point> MomentoxEPI2;                                                                                    // DIAGRAMA MOMENTO X EPI2
std::vector<float> EPI2 = {-10.907, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1.1892, 2, 3, 4, 5, 6, 7, 8, 9, 10.907}; //  VARIAVEIS DE EPI2 PARA DIAGRAMA
std::vector<float> tensaoDiagrama;
std::vector<float> epsilon_c;
float EPI1 = 1.1892; // VALOR FIXO DE EPI1 PARA DIAGRAMA

float graus = 0;
float yMaxSecao = 0;
float yMinSecao = 0;
float yMinArmadura = 0;
float KeyDownDelay = 0.0f;
float KeyDownDelayTime = 0.1f;
int numBarras = 0;
int barras = 0;
int opcao = 0;
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
bool janelaDeformacaoAco = true;
bool janelaDiagramaNormalXMomento = true;
bool entradaDeDadosMateriais = true;
bool janelaSalvar = true;
bool janelaSobre = true;

int numVertices;
float nsd;
float msdx;
float msdy;
float fyk_variavel = 500.0f;
float gama_s_variavel = 1.15f;
float Ep2_variavel = +10.907f;
float Ep1_variavel = -1.189f;
float Es_variavel = 210.0f;
float fyd_variavel;
float epsilon_yd_variavel;
float beta_variavel = 0.85;
float yc_variavel;
float ec2_variavel;
float ecu_variavel;
float fck_variavel;
float epsilon_concreto_ultimo;
float epsilon_concreto_2;

Point centroideInicial;
ImFont *font = nullptr;

void renderizacaoBarras(std::vector<Point>, std::string);

void tamanhoDinamico(const char *nomeJanela, float fatorX, float fatorY, float posX, float posY)
{
    ImGuiIO &io = ImGui::GetIO();

    float screenWidth = io.DisplaySize.x;
    float screenHeight = io.DisplaySize.y;

    float janelaWidght = (io.DisplaySize.x * fatorX) - posX;
    float janelaHeight = (io.DisplaySize.y * fatorY) - posY;

    ImGui::SetNextWindowSize(ImVec2(janelaWidght, janelaHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always);

    ImGui::Begin(nomeJanela, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration);
}

void cantoDireito(const char *nome, float altura, float posY)
{
    ImGuiIO &io = ImGui::GetIO(); // Obtém as dimensões da tela

    // Calcula a largura disponível para a tabela (máximo espaço da tela preta)
    float larguraDisponivel = io.DisplaySize.x * 0.15f; // Ocupa 30% da largura da tela
    float larguraMinima = 150.0f;                       // Largura mínima da tabela
    float larguraTabela = (larguraDisponivel > larguraMinima) ? larguraDisponivel : larguraMinima;

    // Define a posição no canto superior direito
    ImVec2 posicaoTopoDireito = ImVec2(
        io.DisplaySize.x - larguraTabela, // Move 10px para dentro da borda direita
        posY                              // Mantém a posição Y fixa
    );

    // Define a posição e o tamanho da janela
    ImGui::SetNextWindowPos(posicaoTopoDireito, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(larguraTabela, altura), ImGuiCond_Always);

    // Criar a janela fixa no canto superior direito
    ImGui::Begin(nome, nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar);

    // Criando a tabela de pontos da seção transversal
    if (ImGui::BeginTable("TabelaPontos", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, larguraTabela * 0.20f);
        ImGui::TableSetupColumn("x (cm)", ImGuiTableColumnFlags_WidthFixed, larguraTabela * 0.40f);
        ImGui::TableSetupColumn("y (cm)", ImGuiTableColumnFlags_WidthFixed, larguraTabela * 0.40f);
        ImGui::TableHeadersRow();

        // Exemplo de dados (substitua com seus valores reais)
        for (size_t i = 0; i < collectedPoints.size(); i++)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", static_cast<int>(i + 1));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f", collectedPoints[i].x);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.2f", collectedPoints[i].y);
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

/* void cantoDireito(const char* nome, float x, float y, float posY) {
    ImGuiIO& io = ImGui::GetIO(); // Obtém as dimensões da tela

    // Defina o tamanho da janela
    ImVec2 windowSize = ImVec2(x, y); // Ajuste o tamanho conforme necessário

    // Calcula a posição para o canto superior direito
    ImVec2 topRightPos = ImVec2(
        io.DisplaySize.x - windowSize.x, // Largura da tela menos a largura da janela
        posY  // Alinhado ao topo (sem margem)
    );

    // Define a posição e o tamanho da janela
    ImGui::SetNextWindowPos(topRightPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    // Criar a janela fixa no canto superior direito
    ImGui::Begin(nome, nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);


} */

void IniciarInterface()
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

void ShowAboutWindow()
{
    ImGuiIO &io = ImGui::GetIO();

    float width = io.DisplaySize.x;
    float height = io.DisplaySize.y;

    ImVec2 posCentral = ImVec2((io.DisplaySize.x - 500) * 0.5, (io.DisplaySize.y - 300) * 0.5);

    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(posCentral, ImGuiCond_Always);
    tamanhoDinamico("Autores", 0.5, 0.25, 0, 0);

    // Define fundo mais suave
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 0.9f)); // Cinza escuro
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize("SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO").x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

    // Exibe o título
    ImGui::Text("SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO");

    // Linha separadora estética
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Usa a fonte do texto

    // Exibe os nomes e emails com negrito
    ImGui::Text("Desenvolvido por:");
    ImGui::BulletText("Arthur C. Pena - arthur.cunha.pena@ufu.br");
    ImGui::BulletText("Gabriel A. P. Lunarti - gabriel.lunarti@ufu.br");

    ImGui::Spacing();

    ImGui::Text("Orientador:");
    ImGui::BulletText("Prof Dr Eduardo Vicente Wolf Trentini");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX((windowWidth - 80) * 0.5f); // Centraliza o botão
    if (ImGui::Button("Fechar", ImVec2(80, 30)))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::PopStyleColor(); // Restaura a cor original
    ImGui::End();
}

void ShowSecondaryMenuBar()
{
    // Obtém o tamanho da janela principal
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 window_size = ImVec2(io.DisplaySize.x, ImGui::GetFrameHeight());

    // Configura a posição e o tamanho da nova janela
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);

    // Cria uma janela sem decorações com uma barra de menu
    ImGui::Begin("##SecondaryMenuBar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar);

    // Cria a barra de menu dentro da janela
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Seção Transversal"))
        {
            ImGui::Begin("Inserir Dados da Seção Transversal", nullptr,
                         ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove);

            // Input para número de pontos
            ImGui::SetNextItemWidth(80);
            ImGui::InputInt("Número de Pontos", &numVertices);

            // Impede valores negativos e ajuste de tamanho do vetor apenas quando necessário
            if (numVertices < 0)
                numVertices = 0;
            if (numVertices != static_cast<int>(collectedPoints.size()))
            {
                collectedPoints.resize(numVertices);
            }

            ImGui::Spacing(); // Dá um pequeno espaçamento para organização

            // Botão para limpar todos os pontos
            if (ImGui::Button("Limpar"))
            {
                collectedPoints.clear();
                numVertices = 0;
            }

            ImGui::Spacing(); // Adiciona um espaçamento antes da tabela

            // Criando a tabela de pontos
            if (ImGui::BeginTable("Tabela", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("x (cm)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("y (cm)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < collectedPoints.size(); i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", static_cast<int>(i + 1));

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(100);
                    ImGui::InputFloat(("##X" + std::to_string(i)).c_str(), &collectedPoints[i].x, 0.1f, 1.0f, "%.2f");

                    ImGui::TableSetColumnIndex(2);
                    ImGui::SetNextItemWidth(100);
                    ImGui::InputFloat(("##Y" + std::to_string(i)).c_str(), &collectedPoints[i].y, 0.1f, 1.0f, "%.2f");
                }
                ImGui::EndTable();
            }
            ImGui::End();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Materiais"))
        {
            ImGui::SetNextWindowSize(ImVec2(610, 400), ImGuiCond_Always);    // Ajuste os valores conforme necessário
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver); // Posição inicial
            ImGui::Begin("Entrada de Dados de Materiais", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            if (ImGui::BeginTabBar("Tabela de Entrada de Dados de Materiais"))
            {
                if (ImGui::BeginTabItem("Concreto"))

                {
                    ImGui::Text("Tipo de Diagrama:");
                    ImGui::RadioButton("Parábola-Retangulo (NBR 6118:2023)", &opcao, 0);
                    ImGui::RadioButton("Parábola-Retangulo (NBR 6118:2014)", &opcao, 1);

                    if (opcao == 0)
                    {
                        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 160);
                        ImGui::Text("Parâmetros do Concreto");

                        // Criar um grupo para alinhar os elementos automaticamente
                        ImGui::PushItemWidth(70);
                        float pos_x_direita = ImGui::GetWindowWidth() - 160;

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::Text("Fck  = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##Fck", &fck_variavel, 0.0f, 0.0f, "%.2f");
                        ImGui::SameLine();
                        ImGui::Text("MPa");

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::SetNextItemWidth(100);
                        ImGui::Text("γc   = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##γc", &yc_variavel, 0.0f, 0.0f, "%.2f");

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::Text("β     = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##β", &beta_variavel, 0.0f, 0.0f, "%.2f");

                        if (fck_variavel < 0)
                            fck_variavel = 0;

                        if (beta_variavel < 0)
                            beta_variavel = 0;

                        if (yc_variavel < 0)
                            yc_variavel = 0;

                        float fcd_variavel;
                        float expoente_tensao_concreto;
                        int i;
                        float nc;
                        float tensao_maxima;
                        fcd_variavel = fck_variavel / yc_variavel;
                        float maximo;

                        if (fck_variavel <= 40)
                        {
                            nc = 1;
                        }

                        else
                        {
                            nc = pow((40 / fck_variavel), 1.0f / 3.0f);
                        }

                        if (fck_variavel <= 50)
                        {
                            expoente_tensao_concreto = 2;
                            ecu_variavel = 3.5; // c50 ecu
                            ec2_variavel = 2;   // c50 ec2
                        }

                        else
                        {
                            expoente_tensao_concreto = 1.4 + 23.4 * pow((90 - fck_variavel) / 100, 4);
                            ec2_variavel = 2 + 0.085 * pow(fck_variavel - 50, 0.53f);
                            ecu_variavel = 2.6 + 35 * pow((90 - fck_variavel) / 100, 4);
                        }

                        if (ec2_variavel > ecu_variavel)
                        {
                            ec2_variavel = ecu_variavel;
                        }

                        epsilon_c.clear();
                        tensaoDiagrama.clear();
                        float tensaoy;

                        for (float contador = 0; contador <= ecu_variavel; contador += 0.1f)
                        {
                            epsilon_c.push_back(contador);

                            if (contador <= ec2_variavel)
                            {
                                tensaoy = nc * beta_variavel * fcd_variavel * (1 - std::pow((1 - (contador / ec2_variavel)), expoente_tensao_concreto));

                                // Armazena o valor calculado
                                tensao_maxima = tensaoy;
                                maximo = tensaoy;

                                // Exibir os valores para depuração
                                TraceLog(LOG_INFO, "εc = %.2f | Tensão = %.2f", contador, tensaoy);
                            }
                            else
                            {
                                tensaoy = tensao_maxima;
                                maximo = tensaoy;
                            }
                            tensaoDiagrama.push_back(tensaoy);
                        }

                        tensaoDiagrama.push_back(tensao_maxima);
                        epsilon_c.push_back(ecu_variavel);

                        std::vector<float> tensao(epsilon_c.size());
                        std::vector<float> epsilon(epsilon_c.size());

                        for (size_t i = 0; i < epsilon_c.size(); i++)
                        {
                            tensao[i] = tensaoDiagrama[i];
                            epsilon[i] = epsilon_c[i];
                        }

                        ImGui::SetCursorPos(ImVec2(0, 135));
                        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto ABNT:6118:2023", ImVec2(425, 225), ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
                        {
                            if (fck_variavel > 0 && yc_variavel > 0 && beta_variavel > 0)
                            {
                                // eixos do grafico
                                ImPlot::SetupAxis(ImAxis_X1, " εc");
                                ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");

                                if (!epsilon.empty() && epsilon.size() > 1)
                                {
                                    float max_ec = ecu_variavel * 1.1f; // 10% a mais para melhor visualização
                                    float max_sigma = maximo * 1.2f;    // 20% a mais para evitar corte do gráfico

                                    ImPlot::SetupAxesLimits(0, max_ec, 0, max_sigma, ImGuiCond_Always);
                                }

                                ImPlot::SetNextLineStyle(ImVec4(0.53f, 0.81f, 0.98f, 1.0f));

                                if (!epsilon.empty() && epsilon.size() > 1)
                                {

                                    ImPlot::PlotLine("Diagrama Tensão-Deformação Concreto ABNT:6118:2023)", epsilon.data(), tensao.data(), static_cast<int>(epsilon.size()));
                                    ImPlot::Annotation(ec2_variavel, maximo, ImVec4(1, 0, 1, 0), ImVec2(-20, -10), ec2_variavel, " (β *ηc * fcd)  = %.2f MPa", maximo);

                                    float x_values[] = {0, ec2_variavel};
                                    float y_values[] = {maximo, maximo};

                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_values, y_values, 2);

                                    float x_ec2[] = {ec2_variavel, ec2_variavel};
                                    float y_ec2[] = {0, maximo};
                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_ec2, y_ec2, 2);

                                    float x_ecu[] = {ecu_variavel, ecu_variavel};
                                    float y_ecu[] = {0, maximo};
                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_ecu, y_ecu, 2);

                                    float ec2_offset = -20; // Posição normal do rótulo de ec2
                                    float ecu_offset = 20;  // Posição normal do rótulo de ecu

                                    // Se ec2 e ecu estiverem muito próximos, aumentar o espaçamento
                                    if (fabs(ecu_variavel - ec2_variavel) < 0.5f)
                                    {
                                        ec2_offset -= 10; // Move ec2 mais para a esquerda
                                        ecu_offset += 10; // Move ecu mais para a direita
                                    }

                                    ImPlot::Annotation(ec2_variavel, 0, ImVec4(1, 0, 1, 0), ImVec2(ec2_offset, 0), ec2_variavel, "    εc2 = %.2f", ec2_variavel);
                                    ImPlot::Annotation(ecu_variavel, 0, ImVec4(1, 0, 1, 0), ImVec2(ecu_offset, 0), ecu_variavel, "   εcu = %.2f", ecu_variavel);
                                }
                            }
                            else
                            {
                                ImGui::Text("Não há dados suficientes para plotar.");
                            }

                            ImPlot::EndPlot();
                        }
                    }

                    if (opcao == 1)
                    {
                        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 160);
                        ImGui::Text("Parâmetros do Concreto");
                        // organização Yc

                        ImGui::PushItemWidth(70);
                        float pos_x_direita = ImGui::GetWindowWidth() - 160;

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::Text("Fck  = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##Fck", &fck_variavel, 0.0f, 0.0f, "%.2f");
                        ImGui::SameLine();
                        ImGui::Text("MPa");

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::SetNextItemWidth(100);
                        ImGui::Text("γc   = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##γc", &yc_variavel, 0.0f, 0.0f, "%.2f");

                        ImGui::SetCursorPosX(pos_x_direita);
                        ImGui::Text("β     = ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(70);
                        ImGui::InputFloat("##β", &beta_variavel, 0.0f, 0.0f, "%.2f");
                        if (beta_variavel < 0)
                            beta_variavel = 0;

                        if (fck_variavel < 0)
                            fck_variavel = 0;

                        if (yc_variavel < 0)
                            yc_variavel = 0;

                        float fcd_variavel;
                        float expoente_tensao_concreto;

                        int i;
                        float maximo;

                        if (fck_variavel > 0)
                        {
                            fcd_variavel = fck_variavel / yc_variavel;

                            if (fck_variavel <= 50)
                            {
                                expoente_tensao_concreto = 2;
                                ecu_variavel = 3.5; // c50 ecu
                                ec2_variavel = 2;   // c50 ec2
                            }

                            else
                            {
                                expoente_tensao_concreto = 1.4 + 23.4 * pow((90 - fck_variavel) / 100, 4);
                                ec2_variavel = 2 + 0.085 * pow(fck_variavel - 50, 0.53f);
                                ecu_variavel = 2.6 + 35 * pow((90 - fck_variavel) / 100, 4);
                            }

                            epsilon_c.clear();
                            tensaoDiagrama.clear();
                            float tensaoy;
                            float tensao_maxima;

                            for (float contador = 0; contador < ecu_variavel; contador += 0.1f)
                            {
                                epsilon_c.push_back(contador);

                                if (contador <= ec2_variavel)
                                {
                                    tensaoy = beta_variavel * fcd_variavel * (1 - std::pow((1 - (contador / ec2_variavel)), expoente_tensao_concreto));

                                    // Armazena o valor calculado
                                    tensao_maxima = tensaoy;
                                    maximo = tensaoy;

                                    // Exibir os valores para depuração
                                    TraceLog(LOG_INFO, "εc = %.2f | Tensão = %.2f", contador, tensaoy);
                                }
                                else
                                {
                                    tensaoy = tensao_maxima;
                                    maximo = tensaoy;
                                }
                                tensaoDiagrama.push_back(tensaoy);
                            }
                            tensaoDiagrama.push_back(tensao_maxima);
                            epsilon_c.push_back(ecu_variavel);
                        }

                        std::vector<float> tensao(epsilon_c.size());
                        std::vector<float> epsilon(epsilon_c.size());

                        for (size_t i = 0; i < epsilon_c.size(); i++)
                        {
                            tensao[i] = tensaoDiagrama[i];
                            epsilon[i] = epsilon_c[i];
                        }

                        ImGui::SetCursorPos(ImVec2(0, 135));
                        if (ImPlot::BeginPlot("Diagrama Tensão-Deformação Concreto ABNT:6118:2023", ImVec2(425, 225), ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotFlags_NoLegend))
                        {
                            if (fck_variavel > 0 && yc_variavel > 0 && beta_variavel > 0)
                            {
                                // eixos do grafico
                                ImPlot::SetupAxis(ImAxis_X1, " εc");
                                ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");

                                if (!epsilon.empty() && epsilon.size() > 1)
                                {
                                    float max_ec = ecu_variavel * 1.1f; // 10% a mais para melhor visualização
                                    float max_sigma = maximo * 1.2f;    // 20% a mais para evitar corte do gráfico

                                    ImPlot::SetupAxesLimits(0, max_ec, 0, max_sigma, ImGuiCond_Always);
                                }

                                ImPlot::SetNextLineStyle(ImVec4(0.53f, 0.81f, 0.98f, 1.0f));

                                if (!epsilon.empty() && epsilon.size() > 1)
                                {

                                    ImPlot::PlotLine("Diagrama Tensão-Deformação Concreto ABNT:6118:2023)", epsilon.data(), tensao.data(), static_cast<int>(epsilon.size()));
                                    ImPlot::Annotation(ec2_variavel, maximo, ImVec4(1, 0, 1, 0), ImVec2(-20, -10), ec2_variavel, " (β * fcd)  = %.2f MPa", maximo);

                                    float x_values[] = {0, ec2_variavel};
                                    float y_values[] = {maximo, maximo};

                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_values, y_values, 2);

                                    float x_ec2[] = {ec2_variavel, ec2_variavel};
                                    float y_ec2[] = {0, maximo};
                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_ec2, y_ec2, 2);

                                    float x_ecu[] = {ecu_variavel, ecu_variavel};
                                    float y_ecu[] = {0, maximo};
                                    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImPlot::PlotLine("", x_ecu, y_ecu, 2);

                                    float ec2_offset = -20; // Posição normal do rótulo de ec2
                                    float ecu_offset = 20;  // Posição normal do rótulo de ecu

                                    // Se ec2 e ecu estiverem muito próximos, aumentar o espaçamento
                                    if (fabs(ecu_variavel - ec2_variavel) < 0.5f)
                                    {
                                        ec2_offset -= 10; // Move ec2 mais para a esquerda
                                        ecu_offset += 10; // Move ecu mais para a direita
                                    }

                                    ImPlot::Annotation(ec2_variavel, 0, ImVec4(1, 0, 1, 0), ImVec2(ec2_offset, 0), ec2_variavel, "    εc2 = %.2f", ec2_variavel);
                                    ImPlot::Annotation(ecu_variavel, 0, ImVec4(1, 0, 1, 0), ImVec2(ecu_offset, 0), ecu_variavel, "   εcu = %.2f", ecu_variavel);
                                }
                            }
                            else
                            {
                                ImGui::Text("Não há dados suficientes para plotar.");
                            }

                            ImPlot::EndPlot();
                        }
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Armadura Passiva"))
                {
                    // implot math symbol pesquisar latex
                    // ajeitar concreto e simbolos
                    // redimensionar o grafico do aço
                    // colocar os topicos embaixo
                    // tentar deixar mais visivel a cor do grafico
                    //

                    float pos_x_direita = ImGui::GetWindowWidth() - 150;
                    ImGui::SetCursorPosX(pos_x_direita);
                    ImGui::Text("Parâmetros do aço");
                    // organização gama_s

                    ImGui::SetCursorPosX(pos_x_direita);
                    ImGui::Text("fyk = ");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70);
                    ImGui::InputFloat("##fyk", &fyk_variavel, 0.0f, 0.0f, "%.3f");
                    ImGui::SameLine();
                    ImGui::Text("MPa");

                    ImGui::SetCursorPosX(pos_x_direita);
                    ImGui::SetNextItemWidth(100);
                    ImGui::Text("γs  = ");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70);
                    ImGui::InputFloat("##γs", &gama_s_variavel, 0.0f, 0.0f, "%.3f");
                    if (gama_s_variavel < 0)
                        gama_s_variavel = 0;
                    // organização fyk

                    if (fyk_variavel < 0)
                        fyk_variavel = 0;

                    // organização deformação do aço
                    ImGui::SetCursorPosX(pos_x_direita);
                    ImGui::Text("Es  = ");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70);
                    ImGui::InputFloat("##Es", &Es_variavel, 0.0f, 0.0f, "%.3f");
                    ImGui::SameLine();
                    ImGui::Text("GPa");
                    if (Es_variavel < 0)
                        Es_variavel = 0;

                    // ImPlot --
                    ImGui::Dummy(ImVec2(plotSize.x - (plotSize.x - 150), 0)); // usado pra pular espaços, util dms

                    reforco.calculaParametros(fyk_variavel, gama_s_variavel, Es_variavel);
                    std::vector<float> EPItemp = {-10, -reforco.epsilon_yd, reforco.epsilon_yd, 10};
                    float tensaoY[EPItemp.size()];
                    float xEpi[EPItemp.size()];

                    float resultado_positivo = reforco.epsilon_yd;
                    float resultado_negativo = -resultado_positivo;

                    for (size_t i = 0; i < EPItemp.size(); i++)
                    {

                        float EPIvariavel = EPItemp[i];
                        xEpi[i] = EPItemp[i];
                        tensaoY[i] = reforco.tensao(EPIvariavel);
                    }
                    ImGui::SetCursorPos(ImVec2(0, 60));
                    if (ImPlot::BeginPlot("Diagrama Tensão-Deformação (Linear com patamar)", ImVec2((plotSize.x - 200), plotSize.y - 20), ImPlotFlags_NoInputs | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Invert | ImPlotFlags_NoLegend))
                    {
                        ImPlot::SetupAxisLimits(ImAxis_X1, -10.0, 10.0, ImGuiCond_Always);
                        ImPlot::SetupAxisLimits(ImAxis_Y1, -800, 800, ImGuiCond_Always);

                        ImPlot::SetupAxis(ImAxis_X1, " ε por mil");
                        ImPlot::SetupAxis(ImAxis_Y1, " σ (MPa)");

                        // ImPlot::PlotText("-> (-fyk/ ys)", -reforco.epsilon_yd, tensaoY[0], ImVec2(60, 0));
                        // ImPlot::PlotText("(fyk/ ys) <-", reforco.epsilon_yd, tensaoY[3], ImVec2(-60, 0));

                        // -10 -reforco.epsilon_yd -2.5 - tensão[0]
                        // 10 - reforco.espilon_yd, 2.5 - tensaoY[3]

                        float annotation_offset_x = 10; // Ajuste horizontal das anotações
                        float annotation_offset_y = 5;  // Ajuste vertical das anotações

                        // Anotações das tensões fyd e -fyd
                        ImPlot::Annotation(-reforco.epsilon_yd, tensaoY[0], ImVec4(1, 1, 1, 0.4), ImVec2(-annotation_offset_x, -annotation_offset_y), tensaoY[0], "-fyd = %.2f MPa", tensaoY[0]);
                        ImPlot::Annotation(reforco.epsilon_yd, tensaoY[3], ImVec4(1, 1, 1, 0.4), ImVec2(annotation_offset_x, annotation_offset_y), tensaoY[3], "fyd = %.2f MPa", tensaoY[3]);

                        // Anotações das deformações εyd e -εyd
                        ImPlot::Annotation(-reforco.epsilon_yd, 0, ImVec4(1, 1, 1, 0.4), ImVec2(-annotation_offset_x / 2, -10), resultado_negativo, "εyd = %.2f", resultado_negativo);
                        ImPlot::Annotation(reforco.epsilon_yd, 0, ImVec4(1, 1, 1, 0.4), ImVec2(annotation_offset_x / 2, 10), resultado_positivo, "εyd = %.2f", resultado_positivo);

                        float x_eyd[] = {-reforco.epsilon_yd, -reforco.epsilon_yd};
                        float y_eyd[] = {0, tensaoY[0]};
                        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
                        ImPlot::PlotLine("", x_eyd, y_eyd, 2);

                        float x_eyd2[] = {reforco.epsilon_yd, reforco.epsilon_yd};
                        float y_eyd2[] = {0, tensaoY[3]};
                        ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
                        ImPlot::PlotLine("", x_eyd2, y_eyd2, 2);

                        ImPlot::SetNextLineStyle(ImVec4(0.53f, 0.81f, 0.98f, 1.0f));
                        ImPlot::PlotLine("Linha", xEpi, tensaoY, EPItemp.size());

                        ImPlot::EndPlot();
                    }

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Armadura Passiva"))
        {

            ImGui::Begin("Entrada de Dados: Armadura Passiva", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::RadioButton("Uma Barra", &barras, 0);
            ImGui::RadioButton("Linha de Barras", &barras, 1);

            if (barras == 0)
            {
                numBarras = 1;
                ImGui::PushItemWidth(50);
                ImGui::InputFloat("Diâmetro das Barras (mm)", &diametroBarras);
                ImGui::InputFloat("x (cm)", &barrasPosXi);
                ImGui::InputFloat("y (cm)", &barrasPosYi);
                if (diametroBarras < 0)
                    diametroBarras = 0;
                if (ImGui::Button("Adicionar"))
                {
                    if (diametroBarras <= 0)
                    {
                    }
                    else
                    {
                        reforco.AdicionarBarra(barrasPosXi, barrasPosYi, diametroBarras);
                        Point centroide = poligono.centroide();

                        reforco.translacaoCG(reforco.Armaduras, centroide);
                        reforco.RotacionarArmadura(graus);
                    }
                }
                ImGui::SameLine();
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
                ImGui::InputFloat("Diâmetro das Barras (mm)", &diametroBarras);
                ImGui::InputFloat("Xi (cm)", &barrasPosXi);
                ImGui::SameLine();
                ImGui::InputFloat("Yi (cm)", &barrasPosYi);
                ImGui::InputFloat("Xf (cm)", &barrasPosXf);
                ImGui::SameLine();
                ImGui::InputFloat("Yf (cm)", &barrasPosYf);
                if (diametroBarras < 0)
                    diametroBarras = 0;

                float xAdicionado = (barrasPosXf - barrasPosXi) / (numBarras - 1);
                float yAdicionado = (barrasPosYf - barrasPosYi) / (numBarras - 1);
                float valorAdicionadoX = 0;
                float valorAdicionadoY = 0;

                if (ImGui::Button("Adicionar"))
                {
                    if (diametroBarras <= 0)
                    {
                    }
                    else
                    {
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
                ImGui::SameLine();
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
                ImGui::TableSetupColumn("Posição X (cm)");
                ImGui::TableSetupColumn("Posição Y (cm)");
                ImGui::TableSetupColumn("Diâmetro (mm)");
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

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Esforços"))
        {

            ImGui::Begin("Esforços", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::PushItemWidth(50);
            ImGui::InputFloat("Nsd", &nsd);
            ImGui::SameLine();
            ImGui::Text("(kN)");
            ImGui::InputFloat("Msd,x", &msdx);
            ImGui::SameLine();
            ImGui::Text("(kN.m)");

            if (ImGui::Button("Adicionar"))
            {
                combinacao.push_back({nsd, msdx, msdy});
            }
            ImGui::SameLine();
            if (ImGui::Button("Remover"))
            {

                if (!combinacao.empty())
                {
                    combinacao.pop_back(); // Remove a última barra
                }
            }

            ImGui::BeginTable("Tabela", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
            ImGui::TableSetupColumn("Combinacao");
            ImGui::TableSetupColumn("Nsd (kN)");
            ImGui::TableSetupColumn("Msd,x (kN.m)");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < combinacao.size(); ++i)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", static_cast<int>(i + 1));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", combinacao[i].Normal);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", combinacao[i].MsdX);
            }
            ImGui::EndTable();
            ImGui::End();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

struct dadosCereal
{

    float variavel_fck, variavel_gama, variavel_beta;

    template <class Archive>
    void serialize(Archive &ar)
    {

        ar(CEREAL_NVP(variavel_fck), CEREAL_NVP(variavel_gama), CEREAL_NVP(variavel_beta));
    }
};

void loopPrograma()
{
    reforco.AdicionarBarra(-7.f, -17.f, 10.f);
    reforco.AdicionarBarra(+7.f, -17.f, 10.f);

    while (!WindowShouldClose())
    {
        DrawFPS(20, 20);
        KeyDownDelay = KeyDownDelay + GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin(); // substituir

        if (font)
            ImGui::PushFont(font);

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
                ShowAboutWindow();
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ShowSecondaryMenuBar();
        tamanhoDinamico("Grafico", 0.85, 1, 0, 50);
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
        // float xArmadura[ArmadurasFechadas.size()];
        // float yArmadura[ArmadurasFechadas.size()];
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
        if (ImPlot::BeginPlot("Seção Transversal", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
        {
            ImPlot::SetupAxis(ImAxis_X1, " x (cm)");
            ImPlot::SetupAxis(ImAxis_Y1, " y (cm))");

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

        ImGui::End(); // Finaliza a janela do gráfico

        cantoDireito("Pontos da Secao Transversal", 200, 50);

        // dadosCereal salvar;
        /* if (janelaSalvar)
        {

            ImGui::Begin("Aba de Salvamento de Informações");

            ImGui::InputFloat("FCK", &salvar.variavel_fck);
            ImGui::InputFloat("BETA", &salvar.variavel_beta);
            ImGui::InputFloat("GAMA", &salvar.variavel_gama);

            if (ImGui::Button("Salvar"))
            {
                std::ofstream file("dados.json");
                cereal::JSONOutputArchive archive(file);

                archive(CEREAL_NVP(salvar));
            }

            if (ImGui::Button("Carregar"))
            {
                std::ifstream file("dados.json");

                if (file)
                {
                    cereal::JSONInputArchive archive(file);
                    archive(CEREAL_NVP(salvar));
                }
                std::cout << "FCK: " << salvar.variavel_fck
                          << " | BETA: " << salvar.variavel_beta
                          << " | GAMA: " << salvar.variavel_gama << std::endl;
            }
            ImGui::End(); // Finaliza a janela do gráfico
        } */

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
        /*
        if (janelaAcoPassivo)
        {
            ImGui::Begin("Parâmetros do Aço Passivo", &janelaAcoPassivo);
            ImGui::InputFloat("FYK:", &fyk_variavel);
            ImGui::InputFloat("Gama_S:", &gama_s_variavel);
            ImGui::InputFloat("Es:", &Es_variavel);
            ImGui::InputFloat("EP2:", &Ep2_variavel);
            ImGui::InputFloat("Ep1:", &Ep1_variavel);

            if (ImGui::Button("Pressione aqui para o cálculo de parâmetros FYD e Epyd"))
            {
                reforco.calculaParametros(fyk_variavel, gama_s_variavel, Es_variavel);
                fyd_variavel = reforco.fyd;
                epsilon_yd_variavel = reforco.epsilon_yd;
                TraceLog(LOG_INFO, "TraceLog %2.f", fyd_variavel);
                TraceLog(LOG_INFO, "TraceLog %2.f", epsilon_yd_variavel);
                reforco.calculaNormal_Momento(Ep2_variavel, Ep1_variavel);
            }

            ImGui::End();
        }
        */
        /*
        if (janelaDiagramaNormalXMomento)
        {

            ImGui::Begin("Janela de Diagramas de Normal e Momento por variação de EP2");
            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            reforco.calculaParametros(fyk_variavel, gama_s_variavel, Es_variavel);

            std::vector<float> normal(EPI2.size());
            std::vector<float> momento(EPI2.size());
            std::vector<float> EP(EPI2.size());
            for (size_t i = 0; i < EPI2.size(); i++)
            {

            reforco.calculaNormal_Momento(EPI2[i], EPI1);
            NormalxEPI2.emplace_back(EPI2[i], reforco.soma_normal_aco_passivo);
            MomentoxEPI2.emplace_back(EPI2[i], reforco.soma_momento_aco_passivo);

            TraceLog(LOG_INFO, "VALOR %2.f", i);
            TraceLog(LOG_INFO, "Valor de EPI2 X %6.f e NORMAL %.6f", NormalxEPI2[i].x, NormalxEPI2[i].y);
            TraceLog(LOG_INFO, "Valor de EPI2o X %6.f e MOMENTO %.6f", MomentoxEPI2[i].x, MomentoxEPI2[i].y);
            TraceLog(LOG_INFO, "Valor de Deformação de Barra %6.f", reforco.deformacao_barra);


            normal[i] = NormalxEPI2[i].y;
            momento[i] = MomentoxEPI2[i].y;
            EP[i] = EPI2[i];

            }

            if(ImPlot::BeginPlot("Diagrama de Normal e Momento por variação de EPI2", ImVec2(plotSize.x, plotSize.y)))
            {

                ImPlot::PlotLine("Pontos Normal em kN",EP.data(), normal.data(), static_cast<int>(EPI2.size()));
                ImPlot::PlotLine("Pontos Momento em kN/m",EP.data(), momento.data(),  static_cast<int>(EPI2.size()));

                ImPlot::EndPlot();
            }
            ImGui::End();
        }
        */

        /*
        if (janelaDeformacaoAco)
        {
            ImGui::Begin("Janela do gráfico de Deformação do Aço", &janelaDeformacaoAco);
            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            float tensaoY[EPIx.size()];
            float xEpi[EPIx.size()];

            for (size_t i = 0; i < EPIx.size(); i++)
            {

                reforco.calculaParametros(fyk_variavel, gama_s_variavel, Es_variavel);

                float EPIvariavel = EPIx[i];
                xEpi[i] = EPIx[i];
                tensaoY[i] = reforco.tensao(EPIvariavel);
            }

            if (ImPlot::BeginPlot("Gráfico de Deformação do Aço", ImVec2(plotSize.x, plotSize.y)))
            {
                ImPlot::PlotScatter("Pontos", xEpi, tensaoY, EPIx.size());

                ImPlot::PlotLine("Linha", xEpi, tensaoY, EPIx.size());
                ImPlot::EndPlot();
            }

            ImGui::End();
        }
*/
        /*
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
           */

        if (font)
            ImGui::PopFont();

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

void renderizacaoBarras(std::vector<Point> vertices, std::string nomeVerticesBarras)
{
    // verificação se o vetor está vazio, prossegue se não
    if (!vertices.empty())
    {
        // vetores temporários para usar no PlotScatter
        // inicialização com o tamanho do vetor do parâmetro da função
        // inicialização do tipo vector<float> nome(tamanho)
        std::vector<float> xTemp(vertices.size());
        std::vector<float> yTemp(vertices.size());

        // alimentação dos vetores temporários
        for (size_t i = 0; i < vertices.size(); i++)
        {
            xTemp[i] = vertices[i].x;
            yTemp[i] = vertices[i].y;
        }

        // desenho dos pontos
        // metodo data() para referenciar como ponteiro
        // static_cast é uma conversão de tipo size_t (unsigned inteiro) para int
        // não está relacionada a variáveis estáticas

        ImPlot::PlotScatter(nomeVerticesBarras.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vertices.size()));
    }
}