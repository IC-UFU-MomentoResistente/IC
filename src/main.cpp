// https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html 
// https://traineq.org/implot_demo/src/implot_demo.html

#include <iostream>
#include <cmath>
#include <vector>
#include <string>

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

struct VetoresAcimaAbaixo
{
	vector<Ponto> verticesAcima;
	vector<Ponto> verticesAbaixo;
};

struct Poligonal
{
	vector<Ponto> verticesPoligonal;
	vector<Ponto> verticesPoligonalVazada;
	VetoresAcimaAbaixo vetoresAcimaAbaixo;
	VetoresAcimaAbaixo vetoresAcimaAbaixoVazada;
	float yMaximo;
	float yMinimo;
	float hSecao;
};

struct Reforco
{
	vector<Ponto> barras;
};

struct ParametrosConcreto
{
	float multFcd;
	float epsUltimo;
	float epsLimite;
	float nConc;
	float fcd;
};

struct AlturasDeformacao
{
	float hLN;
	float hEpsLimite;
	float hEpsUltimo;
	float xAlpha;
	float xEpsLimite;
	float xEpsUltimo;
};

struct SecaoComposta
{
	Poligonal secao;
	Poligonal poligonoComprimido;
	Reforco armadura;
	ParametrosConcreto parametos;
	AlturasDeformacao alturasDeformacao;
	float normal;
	float momentoYY;
};

void renderizacaoPoligonal(vector<Ponto>, string, string);
void renderizacaoBarras(vector<Ponto>, string);
void exibirDadosSecaoComposta(SecaoComposta secao);
vector<Ponto> adicionaVertices(vector<Ponto>);
Ponto calculaIntersecao(Ponto, Ponto, float);
VetoresAcimaAbaixo corteLinha(vector<Ponto>, float);
float yMaximo(vector<Ponto>);
float yMinimo(vector<Ponto>);
ParametrosConcreto calculaParametrosConcreto(float, float);
AlturasDeformacao calculaAlturasDeformacao(ParametrosConcreto, Poligonal, float, float);
float normalConcretoRetangulo(SecaoComposta, float, float, float);
float momentoConcretoRetangulo(SecaoComposta, float, float, float);
float normalConcretoParabola(SecaoComposta, float, float, float, float);
float momentoConcretoParabola(SecaoComposta, float, float, float, float);
float forcaNormalConcreto(SecaoComposta);
float momentoYYConcreto(SecaoComposta);

int main()
{
	// Declaração dos registros utilizados
	Poligonal retangulo;
	Poligonal poligonoComprimido;
	Reforco armadura;
	ParametrosConcreto parametrosConcreto;
	AlturasDeformacao alturasDeformacao;
	SecaoComposta secaoComposta;

	// Declaração e inicialização dos vértices
	vector<Ponto> verticesColetados = { {-10, 20}, {-10, -20}, {10, -20}, {10, 20} };
	vector<Ponto> verticesVazados = { {5,10}, {5, -10}, {-5, -10}, {-5,10} };
	vector<Ponto> verticesBarras = { {-5,0}, {5,0} };

	// Inicilização de poligonal (retangulo)
	retangulo.verticesPoligonal = verticesColetados;
	retangulo.verticesPoligonalVazada = verticesVazados;
	armadura.barras = verticesBarras;

	// Declaração e inicialização para os parâmetros do concreto
	float fck = 30; // MPa
	float gamaC = 1.4;
	parametrosConcreto = calculaParametrosConcreto(fck, gamaC);

	// Declaração e inicialização para as alturas de deformação para um conjunto de deformações
	float eps1 = -2.1885;
	float eps2 = 10.988;
	retangulo.yMaximo = yMaximo(retangulo.verticesPoligonal);
	retangulo.yMinimo = yMinimo(retangulo.verticesPoligonal);
	retangulo.hSecao = retangulo.yMaximo - retangulo.yMinimo;
	alturasDeformacao = calculaAlturasDeformacao(parametrosConcreto, retangulo, eps1, eps2);

	// Declaração e inicialização para o corte nas alturas de inicialização
	retangulo.vetoresAcimaAbaixo = corteLinha(retangulo.verticesPoligonal, alturasDeformacao.hLN);
	poligonoComprimido.verticesPoligonal = retangulo.vetoresAcimaAbaixo.verticesAcima;
	poligonoComprimido.vetoresAcimaAbaixo = corteLinha(poligonoComprimido.verticesPoligonal, alturasDeformacao.hEpsLimite);

	// Declaração e inicialização do registro SecaoComposta
	secaoComposta.secao = retangulo;
	secaoComposta.poligonoComprimido = poligonoComprimido;
	secaoComposta.armadura = armadura;
	secaoComposta.parametos = parametrosConcreto;
	secaoComposta.alturasDeformacao = alturasDeformacao;
	secaoComposta.normal = -(forcaNormalConcreto(secaoComposta)); // qual unidade
	secaoComposta.momentoYY = momentoYYConcreto(secaoComposta); // qual unidade

	exibirDadosSecaoComposta(secaoComposta);

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

			ImVec2 plotSize = ImGui::GetContentRegionAvail();

			// inicialização do gráfico com os eixos
			if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
			{
				renderizacaoPoligonal(poligonoComprimido.verticesPoligonal, "vComprimido", "pComprimido");
				renderizacaoPoligonal(poligonoComprimido.vetoresAcimaAbaixo.verticesAcima, "vRetangulo", "pRetangulo");
				renderizacaoPoligonal(poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo, "vParabola", "pParabola");
				// renderizacaoPoligonal(retangulo.verticesPoligonal, "Vertices1", "Retangulo");
				// renderizacaoBarras(armadura.barras, "Barras");
				// renderizacaoPoligonal(retangulo.verticesPoligonalVazada, "Vertices2", "RetanguloVazado");
				// renderizacaoPoligonal(retangulo.vetoresAcimaAbaixo.verticesAcima, "vAcima", "Acima");
				// renderizacaoPoligonal(retangulo.vetoresAcimaAbaixo.verticesAbaixo, "vAbaixo", "Abaixo");
				// renderizacaoPoligonal(retangulo.vetoresAcimaAbaixoVazada.verticesAcima, "v2Acima", "2Acima");
				// renderizacaoPoligonal(retangulo.vetoresAcimaAbaixoVazada.verticesAbaixo, "v2Abaico", "2Abaixo");
			}

			ImPlot::EndPlot();

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

void renderizacaoPoligonal(vector<Ponto> vertices, string nomeVertices, string nomePoligonal)
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
		ImPlot::PlotScatter(nomeVertices.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vertices.size()));

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
			ImPlot::PlotLine(nomePoligonal.c_str(), xTempAresta.data(), yTempAresta.data(), static_cast<int>(xTempAresta.size()));
		}
	}
}

void renderizacaoBarras(vector<Ponto> vertices, string nomeVerticesBarras)
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
		ImPlot::PlotScatter(nomeVerticesBarras.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(vertices.size()));
	}
}

void exibirDadosSecaoComposta(SecaoComposta secaoComposta)
{
	cout << "--------------------------------------------\n";
	cout << "Secao: ";

	cout << "\nVertices: ";
	for (size_t i = 0; i < secaoComposta.secao.verticesPoligonal.size(); i++)
	{
		cout << "("
			<< secaoComposta.secao.verticesPoligonal[i].x << ","
			<< secaoComposta.secao.verticesPoligonal[i].y << ") ";
	}

	cout << "\nVertices comprimidos: ";
	for (size_t i = 0; i < secaoComposta.poligonoComprimido.verticesPoligonal.size(); i++)
	{
		cout << "("
			<< secaoComposta.poligonoComprimido.verticesPoligonal[i].x << ","
			<< secaoComposta.poligonoComprimido.verticesPoligonal[i].y << ") ";
	}

	cout << "\nVertices Tensao Retangulo: ";
	for (size_t i = 0; i < secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima.size(); i++)
	{
		cout << "("
			<< secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima[i].x << ","
			<< secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima[i].y << ") ";
	}

	cout << "\nVertices Tensao Parabola: ";
	for (size_t i = 0; i < secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo.size(); i++)
	{
		cout << "("
			<< secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo[i].x << ","
			<< secaoComposta.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo[i].y << ") ";
	}

	cout << endl;
	cout << "Altura Secao: " << secaoComposta.secao.hSecao << endl;
	cout << "yMaximo: " << secaoComposta.secao.yMaximo << endl;
	cout << "yMinimo: " << secaoComposta.secao.yMinimo << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Parametros Concreto: \n";
	cout << "fcd: " << secaoComposta.parametos.fcd << endl;
	cout << "Fator mult: " << secaoComposta.parametos.multFcd << endl;
	cout << "Exp: " << secaoComposta.parametos.nConc << endl;
	cout << "Eps Ultimo: " << secaoComposta.parametos.epsUltimo << endl;
	cout << "Eps Limite " << secaoComposta.parametos.epsLimite << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Alturas Deformacao: \n";
	cout << "hLN: " << secaoComposta.alturasDeformacao.hLN << endl;
	cout << "hEpsLimite: " << secaoComposta.alturasDeformacao.hEpsLimite << endl;
	cout << "hEpsUltimo: " << secaoComposta.alturasDeformacao.hEpsUltimo << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Forca e Momento:\n";
	cout << fixed;
	cout.precision(4);
	cout << "Normal: " << secaoComposta.normal << " qual unidade?" << endl;
	cout << "Momento: " << secaoComposta.momentoYY << " qual unidade?" << endl;
	cout << "--------------------------------------------\n";
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

// Função auxiliar para calcular o ponto de interseção entre um segmento e a linha horizontal
Ponto calculaIntersecao(Ponto p1, Ponto p2, float yCorte) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float t = (yCorte - p1.y) / dy;  // Interpolação linear
	return { p1.x + t * dx, yCorte };
}

// Função para cortar a seção transversal
VetoresAcimaAbaixo corteLinha(vector<Ponto> vertices, float yCorte)
{
	VetoresAcimaAbaixo resultado;

	for (size_t i = 0; i < vertices.size(); ++i) {
		Ponto atual = vertices[i];
		Ponto proximo = vertices[(i + 1) % vertices.size()];  // Laço fechado

		bool atualAcima = atual.y > yCorte;
		bool proximoAcima = proximo.y > yCorte;

		if (atual.y == yCorte)
		{
			resultado.verticesAcima.push_back(atual);
			resultado.verticesAbaixo.push_back(atual);
		}
		else if (atualAcima)
		{
			resultado.verticesAcima.push_back(atual);
		}
		else
		{
			resultado.verticesAbaixo.push_back(atual);
		}

		// Detectar interseção
		if (atualAcima != proximoAcima)
		{
			Ponto intersecao = calculaIntersecao(atual, proximo, yCorte);
			resultado.verticesAcima.push_back(intersecao);
			resultado.verticesAbaixo.push_back(intersecao);
		}
	}

	return resultado;
}

float yMaximo(vector<Ponto> verticesPoligonal)
{
	if (verticesPoligonal.empty())
		throw invalid_argument("O vetor de vertices está vazio.");

	float yMax = verticesPoligonal[0].y;

	for (size_t i = 0; i < verticesPoligonal.size(); i++)
	{
		if (verticesPoligonal[i].y > yMax) yMax = verticesPoligonal[i].y;
	}

	return yMax;
}

float yMinimo(vector<Ponto> verticesPoligonal)
{
	if (verticesPoligonal.empty())
		throw invalid_argument("O vetor de vertices está vazio.");

	float yMin = verticesPoligonal[0].y;

	for (size_t i = 0; i < verticesPoligonal.size(); i++)
	{
		if (verticesPoligonal[i].y < yMin) yMin = verticesPoligonal[i].y;
	}

	return yMin;
}

ParametrosConcreto calculaParametrosConcreto(float fck, float gamaC)
{
	ParametrosConcreto parametros;

	if (fck <= 50)
	{
		parametros.fcd = fck / gamaC;
		parametros.multFcd = 0.85;
		parametros.nConc = 2;
		parametros.epsUltimo = 3.5;
		parametros.epsLimite = 2;
	}
	else
	{
		parametros.fcd = fck / gamaC;
		parametros.multFcd = 0.85 * (1 - (fck - 50) / 200);
		parametros.nConc = 2.6 + 35 * pow((90 - fck) / 100, 4);
		parametros.epsUltimo = 1.4 + 23.4 * pow((90 - fck) / 100, 4);
		parametros.epsLimite = 2 + 0.085 * pow(fck - 50, 0.53);

		if (parametros.epsLimite > parametros.epsUltimo)
		{
			parametros.epsLimite = parametros.epsUltimo;
		}
	}

	return parametros;
}

AlturasDeformacao calculaAlturasDeformacao(ParametrosConcreto parametros, Poligonal secao, float eps1, float eps2)
{
	AlturasDeformacao alturasTemp;
	float k = secao.hSecao / ((eps2 / 1000) - (eps1 / 1000));

	alturasTemp.xAlpha = (-eps1 / 1000) * k;
	alturasTemp.xEpsLimite = ((-parametros.epsLimite / 1000) - (eps1 / 1000)) * k;
	alturasTemp.xEpsUltimo = ((-parametros.epsUltimo / 1000) - (eps1 / 1000)) * k;
	alturasTemp.hLN = secao.yMaximo - alturasTemp.xAlpha;
	alturasTemp.hEpsLimite = secao.yMaximo - alturasTemp.xEpsLimite;
	alturasTemp.hEpsUltimo = secao.yMaximo - alturasTemp.xEpsUltimo;

	return alturasTemp;
}

float normalConcretoRetangulo(SecaoComposta secao, float c1, float c2, float y)
{
	float nctr = secao.parametos.multFcd * secao.parametos.fcd *
		(c1 * y + c2 * y * y / 2);

	return nctr;
}

float momentoConcretoRetangulo(SecaoComposta secao, float c1, float c2, float y)
{
	float mctr = secao.parametos.multFcd * secao.parametos.fcd *
		(c1 * y * y / 2 + c2 * y * y * y / 3);

	return mctr;
}

float normalConcretoParabola(SecaoComposta secao, float xEps2, float c1, float c2, float y)
{
	float g = secao.secao.yMaximo - secao.alturasDeformacao.xAlpha;
	float n1 = secao.parametos.nConc + 1;
	float n2 = secao.parametos.nConc + 2;
	float n3 = secao.parametos.nConc + 3;
	float fcd = secao.parametos.fcd;
	float multFcd = secao.parametos.fcd;
	float nConc = secao.parametos.nConc;
	float eexp = pow(((g + xEps2 - y) / xEps2), n1);

	float nctp = -multFcd * fcd *
		(-(((xEps2 * eexp) * (c1 * n2 + c2 * (g + xEps2 + nConc * y + y))) / (n1 * n2))
			- (c1 * y - (c2 * y * y) / 2));

	return nctp;
}

float momentoConcretoParabola(SecaoComposta secao, float xEps2, float c1, float c2, float y)
{
	float g = secao.secao.yMaximo - secao.alturasDeformacao.xAlpha;
	float n1 = secao.parametos.nConc + 1;
	float n2 = secao.parametos.nConc + 2;
	float n3 = secao.parametos.nConc + 3;
	float fcd = secao.parametos.fcd;
	float multFcd = secao.parametos.fcd;
	float nConc = secao.parametos.nConc;
	float eexp = pow(((g + xEps2 - y) / xEps2), n1);

	float mctp = ((multFcd * fcd) / (6 * n1 * n2 * n3));


	float Mctp = (multFcd * fcd * (3 * c1 * ((n1) * (n2) * (n3)*y * y + 2 * xEps2 * eexp * (((g)
		+xEps2) * (n3)+(3 + 4 * nConc + nConc * nConc) * y)) + 2 * c2 * ((n1) * (n2) * (n3)
			*y * y * y + 3 * xEps2 * eexp * (2 * (g) * (g)+2 * xEps2 * xEps2 + 2 *
				xEps2 * (n1)*y + (2 + 3 * nConc + nConc * nConc) * y * y + 2 * (g) * (2 *
					xEps2 + (n1)*y))))) / (6 * (n1) * (n2) * (n3));

	return Mctp;
}

float forcaNormalConcreto(SecaoComposta secao)
{
	vector<Ponto> vetTempAcima = secao.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = secao.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	float NC = 0;
	float NCTP = 0;
	float NCTR = 0;

	if (vetTempAcima.empty() || vetTempAbaixo.empty())
	{
		NCTP = 0;
		NCTR = 0;
	}

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		float x1 = vetTempAbaixo[i].x;
		float y1 = vetTempAbaixo[i].y;
		float x2;
		float y2;

		if (i + 1 < vetTempAbaixo.size())
		{
			x2 = vetTempAbaixo[i + 1].x;
			y2 = vetTempAbaixo[i + 1].y;
		}
		else
		{
			x2 = vetTempAbaixo[0].x;
			y2 = vetTempAbaixo[0].y;
		}

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float xEps2 = secao.alturasDeformacao.hEpsLimite - secao.alturasDeformacao.hLN;

			float nc1 = normalConcretoParabola(secao, xEps2, c1, c2, y1);
			float nc2 = normalConcretoParabola(secao, xEps2, c2, c2, y2);

			NCTP = NCTP + nc2 - nc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		float x1 = vetTempAcima[i].x;
		float y1 = vetTempAcima[i].y;
		float x2;
		float y2;

		if (i + 1 < vetTempAcima.size())
		{
			x2 = vetTempAcima[i + 1].x;
			y2 = vetTempAcima[i + 1].y;
		}
		else
		{
			x2 = vetTempAcima[0].x;
			y2 = vetTempAcima[0].y;
		}

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float nc1 = normalConcretoRetangulo(secao, c1, c2, y1);
			float nc2 = normalConcretoRetangulo(secao, c1, c2, y2);

			NCTR = NCTR + nc2 - nc1;
		}
	}

	return NC = NCTP + NCTR;
}

float momentoYYConcreto(SecaoComposta secao)
{
	vector<Ponto> vetTempAcima = secao.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = secao.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	float MC = 0;
	float MCTR = 0;
	float MCTP = 0;

	if (vetTempAcima.empty() || vetTempAbaixo.empty())
	{
		MCTP = 0;
		MCTR = 0;
	}

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		float x1 = vetTempAbaixo[i].x;
		float y1 = vetTempAbaixo[i].y;
		float x2;
		float y2;

		if (i + 1 < vetTempAbaixo.size())
		{
			x2 = vetTempAbaixo[i + 1].x;
			y2 = vetTempAbaixo[i + 1].y;
		}
		else
		{
			x2 = vetTempAbaixo[0].x;
			y2 = vetTempAbaixo[0].y;
		}

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float xEps2 = secao.alturasDeformacao.hEpsLimite - secao.alturasDeformacao.hLN;

			float mc1 = momentoConcretoParabola(secao, xEps2, c1, c2, y1);
			float mc2 = momentoConcretoParabola(secao, xEps2, c1, c2, y2);

			MCTP = MCTP + mc2 - mc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		float x1 = vetTempAcima[i].x;
		float y1 = vetTempAcima[i].y;
		float x2;
		float y2;

		if (i + 1 < vetTempAcima.size())
		{
			x2 = vetTempAcima[i + 1].x;
			y2 = vetTempAcima[i + 1].y;
		}
		else
		{
			x2 = vetTempAcima[0].x;
			y2 = vetTempAcima[0].y;
		}

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float xEps2 = secao.alturasDeformacao.hEpsLimite - secao.alturasDeformacao.hLN;

			float mc1 = momentoConcretoRetangulo(secao, c1, c2, y1);
			float mc2 = momentoConcretoRetangulo(secao, c1, c2, y2);

			MCTR = MCTR + mc2 - mc1;
		}
	}

	return MC = MCTP + MCTR;
}