// https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html 
// https://traineq.org/implot_demo/src/implot_demo.html

#define _USE_MATH_DEFINES
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
	float area;
};

struct Reforco
{
	vector<Ponto> barras;
	vector<float> diametros;
};

struct GeometriaSecao
{
	Poligonal secao;
	Poligonal poligonoComprimido;
	Reforco reforco;
};

struct ParametrosConcreto
{
	float multFcd;
	float epsUltimo;
	float epsLimite;
	float nConc;
	float fcd;
};

struct ParametrosAcoPassivo
{
	float fyd;
	float epsYd;
	float es;
};

struct Materiais
{
	ParametrosConcreto concreto;
	ParametrosAcoPassivo acoPassivo;
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

struct EsforcosInternos
{
	float normalConcreto;
	float momentoConcreto;
	float normalAcoPassivo;
	float momentoAcoPassivo;
	float maxCompressao;
	float maxTracao;
};

struct SomatorioEsforcos
{
	float somatorioNormal;
	float somatorioMomento;
};

struct Secao
{
	Materiais materiais;
	GeometriaSecao geometria;
	AlturasDeformacao alturas;
	EsforcosInternos esforcos;
	SomatorioEsforcos somatorio;
};

struct GraficoEsfxDef
{
	vector<float> eps1;
	vector<float> normal;
	vector<float> momento;
};

void renderizacaoPoligonal(vector<Ponto> vertices, string nomeVertices, string nomePoligonal);
void renderizacaoBarras(vector<Ponto> vertices, string nomeVerticesBarras);
void exibirDadosSecaoComposta(Secao secaoComposta);
vector<Ponto> adicionaVertices(vector<Ponto> verticesColetados);
Ponto calculaIntersecao(Ponto p1, Ponto p2, float yCorte);
VetoresAcimaAbaixo corteLinha(vector<Ponto> vertices, float yCorte);
float yMaximo(vector<Ponto> verticesPoligonal);
float yMinimo(vector<Ponto> verticesPoligonal);
float area(vector<Ponto> verticesPoligonal, vector<Ponto> verticesPoligonalVazada);
ParametrosConcreto calculaParametrosConcreto(float fck, float gamaC);
ParametrosAcoPassivo calculaParametrosAcoPassivo(float fyk, float gamaY, float esY);
AlturasDeformacao calculaAlturasDeformacao(Materiais materiais, Poligonal poligono, float eps1, float eps2);
float normalConcretoRetangulo(Materiais materiais, float c1, float c2, float y);
float momentoConcretoRetangulo(Materiais materiais, float c1, float c2, float y);
float normalConcretoParabola(Materiais materiais, AlturasDeformacao alturas, float c1, float c2, float y);
float momentoConcretoParabola(Materiais materiais, AlturasDeformacao alturas, float c1, float c2, float y);
float forcaNormalConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas);
float calcularMomentoConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas);
float tensaoAcoPassivo(Materiais materiais, float epi);
float forcaNormalAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, float eps1, float eps2);
float calcularMomentoAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, float eps1, float eps2);
float forcaMaximaCompressao(Materiais materiais, Poligonal secao, Reforco reforco);
float forcaMaximaTracao(Materiais materiais, Reforco reforco);
float equilibrioNormalSecao(float normalConcreto, float normalAcoPassivo, float Nsd);
float equilibrioMomentoSecao(float normalConcreto, float momentoAcoPassivo, float Msd);
GraficoEsfxDef graficoEsforcoDeformacao(Secao secaoComposta, float eps2);
Materiais construtorFck30CA50();
Secao construtorSecao(vector<Ponto> vertices, vector<Ponto> vVazados, vector<Ponto> vBarras,
	vector<float> dBarras, float Nsd, float eps1, float eps2);
Secao construtorTriangulo(float Nsd, float eps1, float eps2);
Secao construtorSecaoT(float Nsd, float eps1, float eps2);
float calcularSomatorioNormal(Materiais materiais, Poligonal secao, Reforco armadura, float epsA, float epsB, float Nsd);
float calcularSomatorioMomento(Materiais materiais, Poligonal secao, Reforco armadura, float epsA, float epsB);

int main()
{
	// Comparação conjunto de deformações e esforço normal
	float Nsd = -1600;
	float eps1 = -3.5; // -3.5 N1600 // -3.4397 N2000 // -0.49159 N+100
	float eps2 = 1.1154; // 1.1154 N1600 // -0.0800436 N2000 //  N+100
	Secao secaoComposta = construtorSecaoT(Nsd, eps1, eps2);

	exibirDadosSecaoComposta(secaoComposta);

	GraficoEsfxDef esfXdef = graficoEsforcoDeformacao(secaoComposta, eps2);

	// Busca do conjunto de deformações 
	Materiais materiais = construtorFck30CA50();
	Poligonal secao;
	Poligonal secaoComprimida;
	Reforco armadura;
	EsforcosInternos esforcos;

	// Declaração e inicialização dos vértices
	vector<Ponto> vColetados =
	{ {7.5,0 - 30.1}, {10,30 - 30.1}, {20, 40 - 30.1}, {20, 50 - 30.1}, {-20, 50 - 30.1},
		{-20, 40 - 30.1}, {-10,30 - 30.1}, {-7.5,0 - 30.1} };
	vector<Ponto> vVazados = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	vector<Ponto> vBarras = { {5,2.5 - 30.1}, {5,7.5 - 30.1}, {-5, 7.5 - 30.1}, {-5,2.5 - 30.1} };
	vector<float> dBarras = { 1, 1, 1, 1 }; // em cm

	// Declaração e inicialização da poligonal e reforco
	secao.verticesPoligonal = vColetados;
	secao.verticesPoligonalVazada = vVazados;
	secao.yMaximo = yMaximo(secao.verticesPoligonal);
	secao.yMinimo = yMinimo(secao.verticesPoligonal);
	secao.hSecao = secao.yMaximo - secao.yMinimo;
	secao.area = area(secao.verticesPoligonal, secao.verticesPoligonalVazada);
	armadura.barras = vBarras;
	armadura.diametros = dBarras;

	// Compressão e tração máximas
	esforcos.maxCompressao = forcaMaximaCompressao(materiais, secao, armadura);
	esforcos.maxTracao = forcaMaximaTracao(materiais, armadura);

	if (Nsd >= esforcos.maxCompressao && Nsd <= esforcos.maxTracao)
	{
		Materiais materiaisMrd = construtorFck30CA50();
		Poligonal secaoMrd;
		Poligonal secaoComprimidaMrd;
		Reforco armaduraMrd;

		cout << "\nNsd dentro do limite: "
			<< esforcos.maxCompressao << " <= " << Nsd << " <= " << esforcos.maxTracao << endl;

		secaoMrd.verticesPoligonal = vColetados;
		secaoMrd.verticesPoligonalVazada = vVazados;
		secaoMrd.yMaximo = yMaximo(secaoMrd.verticesPoligonal);
		secaoMrd.yMinimo = yMinimo(secaoMrd.verticesPoligonal);
		secaoMrd.hSecao = secaoMrd.yMaximo - secaoMrd.yMinimo;
		secaoMrd.area = area(secaoMrd.verticesPoligonal, secaoMrd.verticesPoligonalVazada);
		armaduraMrd.barras = vBarras;
		armaduraMrd.diametros = dBarras;


		// =================== Verificação somatorio entre a regiao 1-2 ===================
		// Determinação das deformações da curvatura limite entre a região 1-2
		float eps1 = -(materiaisMrd.concreto.epsUltimo);
		float eps2 = 0;

		cout << "\n--------------------------------------------" << endl;
		cout << "Regiao 1-2" << endl;
		cout << "eps1: " << eps1 << endl;
		cout << "eps2: " << eps2 << endl;

		float somatorioNormal12 = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, eps1, eps2, Nsd);

		cout << "sN1-2: " << somatorioNormal12 << endl;


		// =================== Verificação somatorio regiao 2-3 ===================
		// Calculo da dimensao d 
		float yBarrasMin = 0;
		for (size_t i = 0; i < armadura.barras.size(); i++)
		{
			if (armadura.barras[i].y <= yBarrasMin)
				yBarrasMin = vBarras[i].y;
		}

		float d = secaoMrd.yMaximo - yBarrasMin;
		cout << "\n--------------------------------------------" << endl;
		cout << "Regiao 2-3" << endl;
		cout << "d: " << d << endl;

		// Determinação das deformações da curvatura limite entre a região 2-3 
		// eps2 = (eps(Min ap.y) - eps1) * (hSecao/d) + eps1;
		eps1 = -(materiais.concreto.epsUltimo);
		eps2 = ((10 - eps1) * (secaoMrd.hSecao / d)) + eps1;

		cout << "eps1: " << eps1 << endl;
		cout << "eps2: " << eps2 << endl;

		float somatorioNormal23 = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, eps1, eps2, Nsd);

		cout << "sN2-3: " << somatorioNormal23 << endl;


		// =================== Aplicação do método numérico da falsa posição ===================
		int contIteracoes = 0;
		int limiteIteracoes = 100;
		float precisaoSoma = 0.001;
		float somatorioNormal = precisaoSoma + 1;
		float momentoResistente = 0;

		if (somatorioNormal23 <= 0)
		{
			// falsa posição da região III
			// fixa-se eps2 = ((10 - 3.5) * (secaoMrd.hSecao / d)) + 3.5;
			// e varia eps1

			cout << "\n--------------------------------------------" << endl;
			cout << "Regiao III" << endl;

			float epsB = ((10 - (-materiais.concreto.epsUltimo)) * (secaoMrd.hSecao / d)) + (-materiais.concreto.epsUltimo);
			float ak = -(materiais.concreto.epsUltimo);
			float bk = 10;
			float ck = 0;

			float fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, ak, epsB, Nsd);
			float fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, bk, epsB, Nsd);
			//float fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, bk, epsB, Nsd);
			// float fbk = forcaMaximaTracao(materiais, armaduraMrd);

			cout << "\nak: " << ak << endl;
			cout << "bk: " << bk << endl;
			cout << "fak: " << fak << endl;
			cout << "fbk: " << fbk << endl;
			cout << endl;

			// variavel booleana para verificar se há uma raiz entre o intervalo de deformações
			bool valido = (fak * fbk < 0);

			if (!valido)
				cout << "Erro: Metodo da falsa posicao requer fak*fbk < 0 para garantir uma raiz no intervalo";

			while (valido && contIteracoes < limiteIteracoes && abs(somatorioNormal) > precisaoSoma)
			{
				contIteracoes++;

				ck = bk - ((fbk * (bk - ak)) / (fbk - fak));

				float fck = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, ck, epsB, Nsd);

				bool verificaSinal = (fak * fck > 0);

				if (verificaSinal)
				{
					ak = ck;
					fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, ak, epsB, Nsd);
				}
				else
				{
					bk = ck;
					fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, bk, epsB, Nsd);
				}

				somatorioNormal = fck;

				cout << "\nsN: " << somatorioNormal << endl;
				cout << "ak: " << ak << endl;
				cout << "bk: " << bk << endl;
				cout << "ck: " << ck << endl;
			}

			momentoResistente = calcularSomatorioMomento(materiais, secaoMrd, armaduraMrd, ck, epsB);

			cout << "\n------------------RESULTADOS------------------" << endl;
			cout << "contIter: " << contIteracoes << endl;
			cout << "epsA: " << ck << endl;
			cout << "epsB: " << epsB << endl;
			cout << "Mrd: " << momentoResistente << endl;

		}
		else if (somatorioNormal12 <= 0)
		{
			// falsa posição da região II
			// fixa-se eps1 = -3.5
			// e varia eps2 entre eps2 = ((10 - 3.5) * (secaoMrd.hSecao / d)) + 3.5 e eps = 0

			cout << "\n--------------------------------------------" << endl;
			cout << "Regiao II" << endl;

			// Valores para ak e bk iniciais 
			float epsA = -(materiais.concreto.epsUltimo);
			float ak = 0;
			float bk = 10;
			float ck = 0;

			float fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsA, ak, Nsd);
			float fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsA, bk, Nsd);

			cout << "\nak: " << ak << endl;
			cout << "bk: " << bk << endl;
			cout << "fak: " << fak << endl;
			cout << "fbk: " << fbk << endl;
			cout << endl;

			// variavel booleana para verificar se há uma raiz entre o intervalo de deformações
			bool valido = (fak * fbk < 0);

			if (!valido)
				cout << "Erro: Metodo da falsa posicao requer fak*fbk < 0 para garantir uma raiz no intervalo";

			while (valido && contIteracoes < limiteIteracoes && abs(somatorioNormal) > precisaoSoma)
			{
				contIteracoes++;

				ck = bk - ((fbk * (bk - ak)) / (fbk - fak));

				float fck = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsA, ck, Nsd);

				bool verificaSinal = (fak * fck > 0);

				if (verificaSinal)
				{
					ak = ck;
					fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsA, ak, Nsd);
				}
				else
				{
					bk = ck;
					fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsA, bk, Nsd);
				}

				somatorioNormal = fck;

				cout << "\nsN: " << somatorioNormal << endl;
				cout << "ak: " << ak << endl;
				cout << "bk: " << bk << endl;
				cout << "ck: " << ck << endl;
			}

			momentoResistente = calcularSomatorioMomento(materiais, secaoMrd, armaduraMrd, epsA, ck);

			cout << "\n------------------RESULTADOS------------------" << endl;
			cout << "contIter: " << contIteracoes << endl;
			cout << "epsA: " << epsA << endl;
			cout << "epsB: " << ck << endl;
			cout << "Mrd: " << momentoResistente << endl;
		}
		else
		{
			// falsa posição da região I
			// varia-se a inclinação da reta de deformação
			// eps1 varia entre -3.5 e -2
			// eps2 varia entre 0 e -2

			cout << "\n--------------------------------------------" << endl;
			cout << "Regiao I" << endl;

			// Regiao I, o polo de ruina B está sempre encurtado em 2 por mil
			// y - yp = a(x-xp)
			// xp,yp (3*h/7, -2)
			// y1 = yp + a(x-150/7) = -2 + a(-150/7)
			// y2 = yp + a(x-150/7) = -2 + a(50 - 150/7)

			// a0 = 0 - (-3.5) / 50 = 0.07
			// a1 = -2 - (-2) / 50 = 0 

			// Inicializa as variáveis do método numérico
			float ak = 0.07;
			float bk = 0;
			float ck = 0;

			float epsAak = -2 + ak * (-secaoMrd.hSecao * 3 / 7);
			float epsBak = -2 + ak * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);
			float fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsAak, epsBak, Nsd);

			float epsAbk = -2 + bk * (-secaoMrd.hSecao * 3 / 7);
			float epsBbk = -2 + bk * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);
			float fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsAbk, epsBbk, Nsd);

			float epsAck = 0;
			float epsBck = 0;

			cout << "\nak: " << ak << endl;
			cout << "bk: " << bk << endl;
			cout << "epsAak: " << epsAak << endl;
			cout << "epsBak: " << epsAbk << endl;
			cout << "epsAbk: " << epsAbk << endl;
			cout << "epsBbk: " << epsBbk << endl;
			cout << "fak: " << fak << endl;
			cout << "fbk: " << fbk << endl;
			cout << endl;

			// Verifica se há raiz no intervalo
			bool valido = (fak * fbk < 0);

			if (!valido)
				cout << "Erro: Metodo da falsa posicao requer fak*fbk < 0 para garantir uma raiz no intervalo";

			while (valido && contIteracoes < limiteIteracoes && abs(somatorioNormal) > precisaoSoma)
			{
				contIteracoes++;

				// Atualiza ck
				ck = bk - ((fbk * (bk - ak)) / (fbk - fak));
				epsAck = -2 + ck * (-secaoMrd.hSecao * 3 / 7);
				epsBck = -2 + ck * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);

				float fck = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsAck, epsBck, Nsd);

				// Verifica o sinal e ajusta os limites
				if (fak * fck > 0)
				{
					ak = ck;
					float epsAak = -2 + ak * (-secaoMrd.hSecao * 3 / 7);
					float epsBak = -2 + ak * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);
					fak = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsAak, epsBak, Nsd);
				}
				else
				{
					float epsAbk = -2 + bk * (-secaoMrd.hSecao * 3 / 7);
					float epsBbk = -2 + bk * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);
					float fbk = calcularSomatorioNormal(materiais, secaoMrd, armaduraMrd, epsAbk, epsBbk, Nsd);
				}

				somatorioNormal = fck;

				cout << "\nsN: " << somatorioNormal << endl;
				cout << "ak: " << ak << endl;
				cout << "bk: " << bk << endl;
				cout << "ck: " << ck << endl;
			}

			epsAck = -2 + ck * (-secaoMrd.hSecao * 3 / 7);
			epsBck = -2 + ck * (secaoMrd.hSecao - secaoMrd.hSecao * 3 / 7);

			// Cálculo do momento resistente
			momentoResistente = calcularSomatorioMomento(materiais, secaoMrd, armaduraMrd, epsAck, epsBck);

			cout << "\n------------------RESULTADOS------------------" << endl;
			cout << "contIter: " << contIteracoes << endl;
			cout << "ck: " << ck << endl;
			cout << "epsA: " << epsAck << endl;
			cout << "epsB: " << epsBck << endl;
			cout << "Mrd: " << momentoResistente << endl;
		}

		if (contIteracoes == limiteIteracoes)
			cout << "Equacao de equilibrio nao convergiu dentro do limite de iteracoes" << endl;
	}
	else
	{
		cout << "Esforco normal inadimissivel" << endl;
	}

	// Initialization
	//--------------------------------------------------------------------------------------
	cout << endl;
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
				renderizacaoPoligonal(secaoComposta.geometria.poligonoComprimido.verticesPoligonal, "vComprimido", "pComprimido");
				renderizacaoPoligonal(secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima, "vRetangulo", "pRetangulo");
				renderizacaoPoligonal(secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo, "vParabola", "pParabola");
				renderizacaoPoligonal(secaoComposta.geometria.secao.verticesPoligonal, "Vertices1", "Retangulo");
				renderizacaoBarras(secaoComposta.geometria.reforco.barras, "Barras");
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

		bool janelaGraficoDeformacao = true;
		if (janelaGraficoDeformacao)
		{
			ImGui::Begin("Grafico Deformacao x Esforco", &janelaGraficoDeformacao);

			ImVec2 plotSize = ImGui::GetContentRegionAvail();

			if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y)))
			{
				ImPlot::PlotScatter("coordNormal", esfXdef.eps1.data(), esfXdef.normal.data(), static_cast<int>(esfXdef.normal.size()));
				ImPlot::PlotScatter("coordMomento", esfXdef.eps1.data(), esfXdef.momento.data(), static_cast<int>(esfXdef.momento.size()));
				ImPlot::PlotLine("CurvaNormal", esfXdef.eps1.data(), esfXdef.normal.data(), static_cast<int>(esfXdef.normal.size()));
				ImPlot::PlotLine("CurvaMomento", esfXdef.eps1.data(), esfXdef.momento.data(), static_cast<int>(esfXdef.momento.size()));
			}

			ImPlot::EndPlot();

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

void exibirDadosSecaoComposta(Secao secaoComposta)
{
	cout << "--------------------------------------------\n";
	cout << "Secao: ";

	cout << "\nVertices: ";
	for (size_t i = 0; i < secaoComposta.geometria.secao.verticesPoligonal.size(); i++)
	{
		cout << "("
			<< secaoComposta.geometria.secao.verticesPoligonal[i].x << ","
			<< secaoComposta.geometria.secao.verticesPoligonal[i].y << ") ";
	}

	cout << "\nVertices comprimidos: ";
	for (size_t i = 0; i < secaoComposta.geometria.poligonoComprimido.verticesPoligonal.size(); i++)
	{
		cout << "("
			<< secaoComposta.geometria.poligonoComprimido.verticesPoligonal[i].x << ","
			<< secaoComposta.geometria.poligonoComprimido.verticesPoligonal[i].y << ") ";
	}

	cout << "\nVertices Tensao Retangulo: ";
	for (size_t i = 0; i < secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima.size(); i++)
	{
		cout << "("
			<< secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima[i].x << ","
			<< secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAcima[i].y << ") ";
	}

	cout << "\nVertices Tensao Parabola: ";
	for (size_t i = 0; i < secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo.size(); i++)
	{
		cout << "("
			<< secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo[i].x << ","
			<< secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo.verticesAbaixo[i].y << ") ";
	}

	cout << endl;
	cout << "Altura Secao: " << secaoComposta.geometria.secao.hSecao << endl;
	cout << "yMaximo: " << secaoComposta.geometria.secao.yMaximo << endl;
	cout << "yMinimo: " << secaoComposta.geometria.secao.yMinimo << endl;
	cout << "area: " << secaoComposta.geometria.secao.area << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Parametros Concreto: \n";
	cout << "fcd: " << secaoComposta.materiais.concreto.fcd << " MPa" << endl;
	cout << "Fator mult: " << secaoComposta.materiais.concreto.multFcd << endl;
	cout << "Exp: " << secaoComposta.materiais.concreto.nConc << endl;
	cout << "Eps Ultimo: " << secaoComposta.materiais.concreto.epsUltimo << endl;
	cout << "Eps Limite " << secaoComposta.materiais.concreto.epsLimite << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Alturas Deformacao (Concreto): \n";
	cout << "xAlpha: " << secaoComposta.alturas.xAlpha << endl;
	cout << "xEpsLimite: " << secaoComposta.alturas.xEpsLimite << endl;
	cout << "xEpsUltimo: " << secaoComposta.alturas.xEpsUltimo << endl;
	cout << "hLN: " << secaoComposta.alturas.hLN << endl;
	cout << "hEpsLimite: " << secaoComposta.alturas.hEpsLimite << endl;
	cout << "hEpsUltimo: " << secaoComposta.alturas.hEpsUltimo << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Forca e Momento (Concreto): \n";
	cout << fixed;
	cout.precision(4);
	cout << "Normal: " << secaoComposta.esforcos.normalConcreto << " kN" << endl;
	cout << "Momento: " << secaoComposta.esforcos.momentoConcreto << " kN*m" << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Parametros Aco Passivo: \n";
	cout << "fyd: " << secaoComposta.materiais.acoPassivo.fyd << " MPa" << endl;
	cout << "elasY: " << secaoComposta.materiais.acoPassivo.es << " GPa" << endl;
	cout << "epsYd: " << secaoComposta.materiais.acoPassivo.epsYd << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Forca e Momento (Aco Passivo): \n";
	cout << fixed;
	cout.precision(4);
	cout << "Normal: " << secaoComposta.esforcos.normalAcoPassivo << " kN" << endl;
	cout << "Momento: " << secaoComposta.esforcos.momentoAcoPassivo << " kN*m" << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Normal Minima e Maxima: \n";
	cout << fixed;
	cout.precision(4);
	cout << "Maxima compressao: " << secaoComposta.esforcos.maxCompressao << " kN" << endl;
	cout << "Maxima tracao: " << secaoComposta.esforcos.maxTracao << " kN" << endl;
	cout << "--------------------------------------------\n";

	cout << "--------------------------------------------\n";
	cout << "Equilibrio: \n";
	cout << fixed;
	cout.precision(4);
	cout << "Normal: " << secaoComposta.somatorio.somatorioNormal << " kN" << endl;
	cout << "Momento: " << secaoComposta.somatorio.somatorioMomento << " kN*m" << endl;
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

float area(vector<Ponto> verticesPoligonal, vector<Ponto> verticesPoligonalVazada)
{
	float area = 0;
	float areaExterna = 0;
	float areaInterna = 0;

	int nExterna = verticesPoligonal.size();
	for (int i = 0; i < nExterna; i++)
	{
		int j = (i + 1) % nExterna;
		areaExterna += verticesPoligonal[i].x * verticesPoligonal[j].y;
		areaExterna -= verticesPoligonal[j].x * verticesPoligonal[i].y;
	}

	areaExterna = abs(areaExterna) / 2.0f; // cm^2

	int nInterna = verticesPoligonalVazada.size();
	for (int i = 0; i < nInterna; i++)
	{
		int j = (i + 1) % nInterna;
		areaInterna += verticesPoligonalVazada[i].x * verticesPoligonalVazada[j].y;
		areaInterna += verticesPoligonalVazada[j].x * verticesPoligonalVazada[i].x;
	}

	areaInterna = abs(areaInterna) / 2.0f; // cm^2

	area = (areaExterna - areaInterna) / 10000; // m^2

	return area; // m^2
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

ParametrosAcoPassivo calculaParametrosAcoPassivo(float fyk, float gamaY, float esY)
{
	ParametrosAcoPassivo parametros;

	parametros.fyd = fyk / gamaY; // MPa
	parametros.es = esY;
	// Deformação por mil; MPa / GPa
	parametros.epsYd = parametros.fyd / (parametros.es);

	return parametros;
}

AlturasDeformacao calculaAlturasDeformacao(Materiais materiais, Poligonal poligono, float eps1, float eps2)
{
	AlturasDeformacao alturasTemp;
	float k = poligono.hSecao / ((eps2 / 1000) - (eps1 / 1000));

	alturasTemp.xAlpha = (-eps1 / 1000) * k;
	alturasTemp.xEpsLimite = ((-materiais.concreto.epsLimite / 1000) - (eps1 / 1000)) * k;
	alturasTemp.xEpsUltimo = ((-materiais.concreto.epsUltimo / 1000) - (eps1 / 1000)) * k;
	alturasTemp.hLN = poligono.yMaximo - alturasTemp.xAlpha;
	alturasTemp.hEpsLimite = poligono.yMaximo - alturasTemp.xEpsLimite;
	alturasTemp.hEpsUltimo = poligono.yMaximo - alturasTemp.xEpsUltimo;

	return alturasTemp;
}

float normalConcretoRetangulo(Materiais materiais, float c1, float c2, float y)
{
	float nctr = materiais.concreto.multFcd * materiais.concreto.fcd *
		(c1 * y + c2 * y * y / 2);

	return nctr * 1000; // kN
}

float momentoConcretoRetangulo(Materiais materiais, float c1, float c2, float y)
{
	float mctr = materiais.concreto.multFcd * materiais.concreto.fcd *
		(c1 * y * y / 2 + c2 * y * y * y / 3);

	return mctr * 1000; // kN
}

float normalConcretoParabola(Materiais materiais, AlturasDeformacao alturas, float c1, float c2, float y)
{
	float g = alturas.hLN / 100; // m
	float xEpc2 = (alturas.hEpsLimite - alturas.hLN) / 100; // m
	float n1 = materiais.concreto.nConc + 1;
	float n2 = materiais.concreto.nConc + 2;
	float n3 = materiais.concreto.nConc + 3;
	float fcd = materiais.concreto.fcd; // MPa
	float multFcd = materiais.concreto.multFcd;
	float nConc = materiais.concreto.nConc;
	float eexp = pow(((g + xEpc2 - y) / xEpc2), n1);

	float nctp = -multFcd * fcd *
		(-((xEpc2 * eexp * (c1 * n2 + c2 * (g + xEpc2 + nConc * y + y))) / (n1 * n2)) -
			c1 * y - (c2 * y * y) / 2);

	return nctp * 1000;
}

float momentoConcretoParabola(Materiais materiais, AlturasDeformacao alturas, float c1, float c2, float y)
{
	float g = alturas.hLN / 100; // m
	float xEpc2 = (alturas.hEpsLimite - alturas.hLN) / 100; // m
	float n1 = materiais.concreto.nConc + 1;
	float n2 = materiais.concreto.nConc + 2;
	float n3 = materiais.concreto.nConc + 3;
	float fcd = materiais.concreto.fcd;
	float multFcd = materiais.concreto.multFcd;
	float nConc = materiais.concreto.nConc;
	float eexp = pow(((g + xEpc2 - y) / xEpc2), n1);

	float mctp = (multFcd * fcd * (3 * c1 * ((n1) * (n2) * (n3)*y * y + 2 * xEpc2 * eexp * (((g)
		+xEpc2) * (n3)+(3 + 4 * nConc + nConc * nConc) * y)) + 2 * c2 * ((n1) * (n2) * (n3)
			*y * y * y + 3 * xEpc2 * eexp * (2 * (g) * (g)+2 * xEpc2 * xEpc2 + 2 *
				xEpc2 * (n1)*y + (2 + 3 * nConc + nConc * nConc) * y * y + 2 * (g) * (2 *
					xEpc2 + (n1)*y))))) / (6 * (n1) * (n2) * (n3));

	return mctp * 1000;
}

float forcaNormalConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas)
{
	vector<Ponto> vetTempAcima = pComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = pComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	float NC = 0;
	float NCTP = 0;
	float NCTR = 0;

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		int j = i + 1;
		float x1 = vetTempAbaixo[i].x;
		float y1 = vetTempAbaixo[i].y;
		float x2;
		float y2;

		if (j < vetTempAbaixo.size())
		{
			x2 = vetTempAbaixo[j].x;
			y2 = vetTempAbaixo[j].y;
		}
		else
		{
			x2 = vetTempAbaixo[0].x;
			y2 = vetTempAbaixo[0].y;
		}

		// m
		x1 = x1 / 100;
		x2 = x2 / 100;
		y1 = y1 / 100;
		y2 = y2 / 100;

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
			float c2 = (x1 - x2) / (y1 - y2); // adimensional

			float nc1 = normalConcretoParabola(materiais, alturas, c1, c2, y1);
			float nc2 = normalConcretoParabola(materiais, alturas, c1, c2, y2);

			NCTP = NCTP + nc2 - nc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		int j = i + 1;
		float x1 = vetTempAcima[i].x;
		float y1 = vetTempAcima[i].y;
		float x2;
		float y2;

		if (j < vetTempAcima.size())
		{
			x2 = vetTempAcima[j].x;
			y2 = vetTempAcima[j].y;
		}
		else
		{
			x2 = vetTempAcima[0].x;
			y2 = vetTempAcima[0].y;
		}

		x1 = x1 / 100;
		x2 = x2 / 100;
		y1 = y1 / 100;
		y2 = y2 / 100;

		// Inserir limite de aproximação do zero
		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float nc1 = normalConcretoRetangulo(materiais, c1, c2, y1);
			float nc2 = normalConcretoRetangulo(materiais, c1, c2, y2);

			NCTR = NCTR + nc2 - nc1;
		}
	}

	return NC = -(NCTP + NCTR);
}

float calcularMomentoConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas)
{
	vector<Ponto> vetTempAcima = pComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = pComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	float MC = 0;
	float MCTR = 0;
	float MCTP = 0;

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		int j = i + 1;
		float x1 = vetTempAbaixo[i].x;
		float y1 = vetTempAbaixo[i].y;
		float x2;
		float y2;

		if (j < vetTempAbaixo.size())
		{
			x2 = vetTempAbaixo[j].x;
			y2 = vetTempAbaixo[j].y;
		}
		else
		{
			x2 = vetTempAbaixo[0].x;
			y2 = vetTempAbaixo[0].y;
		}

		x1 = x1 / 100;
		x2 = x2 / 100;
		y1 = y1 / 100;
		y2 = y2 / 100;

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float mc1 = momentoConcretoParabola(materiais, alturas, c1, c2, y1);
			float mc2 = momentoConcretoParabola(materiais, alturas, c1, c2, y2);

			MCTP = MCTP + mc2 - mc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		int j = i + 1;
		float x1 = vetTempAcima[i].x;
		float y1 = vetTempAcima[i].y;
		float x2;
		float y2;

		if (j < vetTempAcima.size())
		{
			x2 = vetTempAcima[j].x;
			y2 = vetTempAcima[j].y;
		}
		else
		{
			x2 = vetTempAcima[0].x;
			y2 = vetTempAcima[0].y;
		}

		x1 = x1 / 100;
		x2 = x2 / 100;
		y1 = y1 / 100;
		y2 = y2 / 100;

		if (y1 - y2 != 0)
		{
			float c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			float c2 = (x1 - x2) / (y1 - y2);

			float mc1 = momentoConcretoRetangulo(materiais, c1, c2, y1);
			float mc2 = momentoConcretoRetangulo(materiais, c1, c2, y2);

			MCTR = MCTR + mc2 - mc1;
		}
	}

	return MC = MCTP + MCTR;
}

float tensaoAcoPassivo(Materiais materiais, float epi)
{
	float tensaoAcoPassivo;

	if (epi > 10.001 || epi < -10.001)
		tensaoAcoPassivo = 0;
	else if (epi <= -materiais.acoPassivo.epsYd)
		tensaoAcoPassivo = -materiais.acoPassivo.fyd;
	else if (epi >= materiais.acoPassivo.epsYd)
		tensaoAcoPassivo = materiais.acoPassivo.fyd;
	else
		tensaoAcoPassivo = (materiais.acoPassivo.es * epi);
	// elasY em GPa e epi em pot mil

	return tensaoAcoPassivo;
}

float forcaNormalAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, float eps1, float eps2)
{
	float NY = 0;

	float k = (eps2 - eps1) / secao.hSecao;

	float deformacaoBarra = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		if (eps2 < 10)
		{
			deformacaoBarra = (k * (secao.yMaximo - reforco.barras[i].y)) + eps1;
		}
		else
		{
			deformacaoBarra = (k * (secao.yMaximo - reforco.barras[i].y)) + eps1;

			if (deformacaoBarra > 10)
				deformacaoBarra = 10;
		}

		float tensao = tensaoAcoPassivo(materiais, deformacaoBarra); // MPa

		float areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		float ny = tensao * areaBarra / 10; // kN

		NY += ny; // kN
	}

	return NY;
}

float calcularMomentoAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, float eps1, float eps2)
{
	float MY = 0;

	float k = (eps2 - eps1) / secao.hSecao;

	float deformacaoBarra = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		if (eps2 < 10)
		{
			deformacaoBarra = (k * (secao.yMaximo - reforco.barras[i].y)) + eps1;
		}
		else
		{
			deformacaoBarra = (k * (secao.yMaximo - reforco.barras[i].y)) + eps1;

			if (deformacaoBarra > 10)
				deformacaoBarra = 10;
		}

		float tensao = tensaoAcoPassivo(materiais, deformacaoBarra);

		float areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4;

		float ny = tensao * areaBarra / 10; // kN
		float my = -ny * (reforco.barras[i].y / 100); // kN*cm^2/100 = kN*m

		MY += my; // kN*m
	}

	return MY;
}

float forcaMaximaCompressao(Materiais materiais, Poligonal secao, Reforco reforco)
{
	float NMIN = 0;
	float Nap = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		float tensao = tensaoAcoPassivo(materiais, -(materiais.concreto.epsLimite));

		float areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		float nap = tensao * areaBarra / 10; // kN

		Nap += nap;
	}

	return NMIN = -(secao.area * materiais.concreto.fcd * materiais.concreto.multFcd) * 1000 + Nap;
}

float forcaMaximaTracao(Materiais materiais, Reforco reforco)
{
	float NMAX = 0;
	float Nap = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		float tensao = tensaoAcoPassivo(materiais, 10.0f);

		float areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		float nap = tensao * areaBarra / 10; // kN

		Nap += nap;
	}

	return NMAX = Nap;
}

float equilibrioNormalSecao(float normalConcreto, float normalAcoPassivo, float Nsd)
{
	return normalConcreto + normalAcoPassivo - Nsd;
}

float equilibrioMomentoSecao(float momentoConcreto, float momentoAcoPassivo, float Msd)
{
	return momentoConcreto + momentoAcoPassivo - Msd;
}

GraficoEsfxDef graficoEsforcoDeformacao(Secao secaoComposta, float eps2)
{
	GraficoEsfxDef temp;

	const int tamanho = 36;
	vector<float> EPS1(tamanho);
	vector<float> normal(tamanho);
	vector<float> momento(tamanho);

	for (int i = 0; i < tamanho; i++)
		EPS1[i] = -(0.0f + i * 0.1f);

	for (int i = 0; i < tamanho; i++)
	{
		secaoComposta.alturas =
			calculaAlturasDeformacao(secaoComposta.materiais, secaoComposta.geometria.secao, EPS1[i], eps2);

		secaoComposta.geometria.secao.vetoresAcimaAbaixo =
			corteLinha(secaoComposta.geometria.secao.verticesPoligonal, secaoComposta.alturas.hLN);

		secaoComposta.geometria.poligonoComprimido.verticesPoligonal =
			secaoComposta.geometria.secao.vetoresAcimaAbaixo.verticesAcima;

		secaoComposta.geometria.poligonoComprimido.vetoresAcimaAbaixo =
			corteLinha(secaoComposta.geometria.poligonoComprimido.verticesPoligonal, secaoComposta.alturas.hEpsLimite);

		normal[i] = forcaNormalConcreto(secaoComposta.materiais, secaoComposta.geometria.poligonoComprimido, secaoComposta.alturas);
		momento[i] = calcularMomentoConcreto(secaoComposta.materiais, secaoComposta.geometria.poligonoComprimido, secaoComposta.alturas);
	}

	temp.eps1 = EPS1;
	temp.normal = normal;
	temp.momento = momento;

	return temp;
}

Materiais construtorFck30CA50()
{
	float fck = 30; //MPa
	float gamaC = 1.4;
	ParametrosConcreto concreto = calculaParametrosConcreto(fck, gamaC);

	float fyk = 500; // MPa
	float gamaS = 1.15;
	float es = 210; // GPa
	ParametrosAcoPassivo acoPassivo = calculaParametrosAcoPassivo(fyk, gamaS, es);

	Materiais temp = { concreto, acoPassivo };

	return temp;
}

Secao construtorSecao(vector<Ponto> vertices, vector<Ponto> vVazados, vector<Ponto> vBarras,
	vector<float> dBarras, float Nsd, float eps1, float eps2)
{
	// Declaração dos registros utilizados
	Poligonal poligono;
	Poligonal poligonoComprimido;
	Reforco armadura;
	GeometriaSecao geometria;
	ParametrosConcreto parametrosConcreto;
	ParametrosAcoPassivo parametrosAcoPassivo;
	Materiais materiais = construtorFck30CA50();
	AlturasDeformacao alturasDeformacao;
	EsforcosInternos esforcos;
	SomatorioEsforcos somatorios;
	Secao secaoComposta;

	// Inicilização de poligonal (poligono)
	poligono.verticesPoligonal = vertices;
	poligono.verticesPoligonalVazada = vVazados;
	poligono.yMaximo = yMaximo(poligono.verticesPoligonal); // cm
	poligono.yMinimo = yMinimo(poligono.verticesPoligonal); // cm
	poligono.hSecao = poligono.yMaximo - poligono.yMinimo; // cm
	poligono.area = area(poligono.verticesPoligonal, poligono.verticesPoligonalVazada); // m^2
	armadura.barras = vBarras;
	armadura.diametros = dBarras;

	// Declaração e inicialização para as alturas de deformação para um conjunto de deformações
	alturasDeformacao = calculaAlturasDeformacao(materiais, poligono, eps1, eps2);

	// Declaração e inicialização para o corte nas alturas de inicialização
	poligono.vetoresAcimaAbaixo = corteLinha(poligono.verticesPoligonal, alturasDeformacao.hLN);

	if (eps1 < 0)
	{
		poligonoComprimido.verticesPoligonal = poligono.vetoresAcimaAbaixo.verticesAcima;
		poligonoComprimido.vetoresAcimaAbaixo = corteLinha(poligonoComprimido.verticesPoligonal, alturasDeformacao.hEpsLimite);
		poligonoComprimido.verticesPoligonalVazada = { {0,0} };
		poligonoComprimido.vetoresAcimaAbaixoVazada.verticesAcima = { {0,0} };
		poligonoComprimido.vetoresAcimaAbaixoVazada.verticesAbaixo = { {0,0} };
		poligonoComprimido.yMaximo = yMaximo(poligonoComprimido.verticesPoligonal);
		poligonoComprimido.yMinimo = yMinimo(poligonoComprimido.verticesPoligonal);
		poligonoComprimido.hSecao = poligonoComprimido.yMaximo - poligonoComprimido.yMinimo;
		poligonoComprimido.area = area(poligonoComprimido.verticesPoligonal, poligonoComprimido.verticesPoligonalVazada);
	}

	// Declaração e inicialização da geometria
	geometria.secao = poligono;
	geometria.reforco = armadura;
	geometria.poligonoComprimido = poligonoComprimido;

	// Declaração e inicialização dos esforços
	esforcos.maxCompressao = forcaMaximaCompressao(materiais, poligono, armadura);
	esforcos.maxTracao = forcaMaximaTracao(materiais, armadura);
	esforcos.normalConcreto = forcaNormalConcreto(materiais, poligonoComprimido, alturasDeformacao);
	esforcos.momentoConcreto = calcularMomentoConcreto(materiais, poligonoComprimido, alturasDeformacao);
	esforcos.normalAcoPassivo = forcaNormalAcoPassivo(materiais, poligono, armadura, eps1, eps2);
	esforcos.momentoAcoPassivo = calcularMomentoAcoPassivo(materiais, poligono, armadura, eps1, eps2);

	// Declaração e inicialização dos somatorios
	somatorios.somatorioNormal = equilibrioNormalSecao(esforcos.normalConcreto, esforcos.normalAcoPassivo, Nsd);
	somatorios.somatorioMomento = equilibrioMomentoSecao(esforcos.momentoConcreto, esforcos.momentoAcoPassivo, 0);

	// Declaração e inicialização do registro Secao
	secaoComposta = { materiais, geometria, alturasDeformacao, esforcos, somatorios };

	return secaoComposta;
}

Secao construtorTriangulo(float Nsd, float eps1, float eps2)
{
	/*
	float Nsd = -300;
	float eps1 = -3.5;
	float eps2 = 0.85211;
	*/

	// Declaração e inicialização dos vértices
	vector<Ponto> vColetados = { {0,20 - 13.33 + 20}, {-10,-20 - 13.33 + 20}, {10,-20 - 13.33 + 20} };
	vector<Ponto> vVazados = { {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	vector<Ponto> vBarras = { {-7, -17 - 13.33 + 20}, {7, -17 - 13.33 + 20} };
	vector<float> dBarras = { 1, 1 }; // em cm

	Secao secaoComposta = construtorSecao(vColetados, vVazados, vBarras, dBarras, Nsd, eps1, eps2);

	return secaoComposta;
}

Secao construtorSecaoT(float Nsd, float eps1, float eps2)
{
	/*
	float Nsd = -1600;
	float eps1 = -3.5;
	float eps2 = 1.1154;
	*/

	// Declaração e inicialização dos vértices
	vector<Ponto> vColetados =
	{ {7.5,0 - 30.1}, {10,30 - 30.1}, {20, 40 - 30.1}, {20, 50 - 30.1}, {-20, 50 - 30.1},
		{-20, 40 - 30.1}, {-10,30 - 30.1}, {-7.5,0 - 30.1} };
	vector<Ponto> vVazados = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	vector<Ponto> vBarras = { {5,2.5 - 30.1}, {5,7.5 - 30.1}, {-5, 7.5 - 30.1}, {-5,2.5 - 30.1} };
	vector<float> dBarras = { 1, 1, 1, 1 }; // em cm

	Secao secaoComposta = construtorSecao(vColetados, vVazados, vBarras, dBarras, Nsd, eps1, eps2);

	return secaoComposta;
}

float calcularSomatorioNormal(Materiais materiais, Poligonal secao, Reforco armadura, float epsA, float epsB, float Nsd)
{
	float somatorioNormal = 0;

	if (epsA == epsB)
	{
		return somatorioNormal = forcaMaximaCompressao(materiais, secao, armadura);
	}

	// Determinação das alturas relativas à curvatura limite entre a região
	AlturasDeformacao alturas = calculaAlturasDeformacao(materiais, secao, epsA, epsB);

	//cout << "\nxln: " << alturas.xAlpha << endl;
	//cout << "xEpc2: " << alturas.xEpsLimite << endl;
	//cout << "xEpcu: " << alturas.xEpsUltimo << endl;
	//cout << "hln: " << alturas.hLN << endl;
	//cout << "hEpc2: " << alturas.hEpsLimite << endl;
	//cout << "hEcpu: " << alturas.hEpsUltimo << endl;

	// Divisão da secao entre as áreas comprimidas, comprimidas parabola e comprimidas retangulo da regiao
	Poligonal secaoComprimida;
	secao.vetoresAcimaAbaixo = corteLinha(secao.verticesPoligonal, alturas.hLN);
	secaoComprimida.verticesPoligonal = secao.vetoresAcimaAbaixo.verticesAcima;
	secaoComprimida.vetoresAcimaAbaixo = corteLinha(secaoComprimida.verticesPoligonal, alturas.hEpsLimite);

	// Verificação do esforco normal e somatorio no limite entre a regiao

	float normalConcreto = forcaNormalConcreto(materiais, secaoComprimida, alturas);
	float normalAcoPassivo = forcaNormalAcoPassivo(materiais, secao, armadura, epsA, epsB);
	somatorioNormal = equilibrioNormalSecao(normalConcreto, normalAcoPassivo, Nsd);

	//cout << "normal concreto: " << normalConcreto << endl;
	//cout << "normal aco: " << normalAcoPassivo << endl;
	//cout << "somatorio: " << somatorioNormal << endl;

	return somatorioNormal;
}

float calcularSomatorioMomento(Materiais materiais, Poligonal secao, Reforco armadura, float epsA, float epsB)
{
	// Determinação das alturas relativas à curvatura limite entre a região
	AlturasDeformacao alturas = calculaAlturasDeformacao(materiais, secao, epsA, epsB);

	//cout << "\nxln: " << alturas.xAlpha << endl;
	//cout << "xEpc2: " << alturas.xEpsLimite << endl;
	//cout << "xEpcu: " << alturas.xEpsUltimo << endl;
	//cout << "hln: " << alturas.hLN << endl;
	//cout << "hEpc2: " << alturas.hEpsLimite << endl;
	//cout << "hEcpu: " << alturas.hEpsUltimo << endl;

	// Divisão da secao entre as áreas comprimidas, comprimidas parabola e comprimidas retangulo da regiao
	Poligonal secaoComprimida;
	secao.vetoresAcimaAbaixo = corteLinha(secao.verticesPoligonal, alturas.hLN);
	secaoComprimida.verticesPoligonal = secao.vetoresAcimaAbaixo.verticesAcima;
	secaoComprimida.vetoresAcimaAbaixo = corteLinha(secaoComprimida.verticesPoligonal, alturas.hEpsLimite);

	// Verificação do esforco normal e somatorio no limite entre a regiao

	float momentoConcreto = calcularMomentoConcreto(materiais, secaoComprimida, alturas);
	float momentoAcoPassivo = calcularMomentoAcoPassivo(materiais, secao, armadura, epsA, epsB);
	float somatorioMomento = equilibrioMomentoSecao(momentoConcreto, momentoAcoPassivo, 0);

	//cout << "normal concreto: " << normalConcreto << endl;
	//cout << "normal aco: " << normalAcoPassivo << endl;
	//cout << "somatorio: " << somatorioNormal << endl;

	return somatorioMomento;
}