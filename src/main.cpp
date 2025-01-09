// https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html 
// https://traineq.org/implot_demo/src/implot_demo.html

#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

using namespace std;

struct Ponto
{
	float x;
	float y;
	Ponto(float coordX, float coordY) : x(coordX), y(coordY) {}
};

struct Poligonal
{
	vector<Ponto> verticesPoligonal;
};

void renderizacaoVertices(const vector<Ponto>&);
vector<Ponto> adicionaVertices(vector<Ponto> verticesColetados);

int main()
{
	Poligonal retangulo;
	vector<Ponto> verticesColetados = { {-10, 20}, {-10, -20}, {10, -20}, {10, 20} };

	retangulo.verticesPoligonal = adicionaVertices(verticesColetados);

	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "IC - FCO - Momentos Resistentes");
	SetTargetFPS(60);
	rlImGuiSetup(true);
	ImPlot::CreateContext();

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		// start ImGui Conent
		rlImGuiBegin();

		bool janelaSecao = true;
		if (janelaSecao)
		{
			// inicia a janela no ImGui
			ImGui::Begin("Secao Transversal", &janelaSecao);

			renderizacaoVertices(retangulo.verticesPoligonal);

			// finaliza a janela no ImGui
			ImGui::End();
		}

		// end ImGui Content
		rlImGuiEnd();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	ImPlot::DestroyContext();
	rlImGuiShutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

void renderizacaoVertices(const vector<Ponto>& vertices)
{
	// Obter o tamanho disponível para o gráfico
	ImVec2 plotSize = ImGui::GetContentRegionAvail();

	// inicialização do gráfico com os eixos
	if (ImPlot::BeginPlot("Gráfico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
	{
		// verificação se o vetor está vazio, prossegue se não
		if (!vertices.empty())
		{
			// vetores temporários para usar no PlotScatter
			// inicialização com o tamanho do vetor do parâmetro da função
			// inicialização do tipo vector<float> nome(tamanho)
			vector<float> xTemp(vertices.size());
			vector<float> yTemp(vertices.size());

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
			ImPlot::PlotScatter("Vertices", xTemp.data(), yTemp.data(), static_cast<int>(vertices.size()));

			// verificação se há três pontos para fechar uma poligonal
			if (vertices.size() > 2)
			{
				vector<float> xTempAresta(vertices.size() + 1);
				vector<float> yTempAresta(vertices.size() + 1);

				// alimentação dos vetores temporários
				for (size_t i = 0; i < vertices.size(); i++)
				{
					xTempAresta[i] = vertices[i].x;
					yTempAresta[i] = vertices[i].y;
				}

				// fecha a poligonal ao fazer com que o último elemento seja igual ao primeiro
				xTempAresta[vertices.size()] = vertices[0].x;
				yTempAresta[vertices.size()] = vertices[0].y;

				// desenho das arestas
				ImPlot::PlotLine("Poligonal", xTempAresta.data(), yTempAresta.data(), static_cast<int>(xTempAresta.size()));
			}
		}

		ImPlot::EndPlot();
	}
}

vector<Ponto> adicionaVertices(vector<Ponto> verticesColetados)
{
	vector<Ponto> verticesTemp;

	for (size_t i = 0; i < verticesColetados.size(); i++)
	{
		// variáveis float para compor a struct Ponto
		float pontoXi = verticesColetados[i].x;
		float pontoYi = verticesColetados[i].y;

		// alimentação do vetor temporário às coordenadas temporárias criadas
		verticesTemp.push_back(Ponto(pontoXi, pontoYi));

		cout << "Verificacao funcao adiciona em [" << i << "]." << endl;
		cout << verticesTemp[i].x << endl;
		cout << verticesTemp[i].y << endl;
	}

	// retorno do vetor temporário
	return verticesTemp;
}