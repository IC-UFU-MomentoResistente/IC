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
	double x;
	double y;
	Ponto(double coordX, double coordY) : x(coordX), y(coordY) {}
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
	double yMaximo;
	double yMinimo;
	double hSecao;
	double area;
};

struct Reforco
{
	vector<Ponto> barras;
	vector<double> diametros;
};

struct GeometriaSecao
{
	Poligonal secao;
	Poligonal poligonoComprimido;
	Reforco reforco;
};

struct ParametrosConcreto
{
	double multFcd;
	double epsUltimo;
	double epsLimite;
	double nConc;
	double fcd;
};

struct ParametrosAcoPassivo
{
	double fyd;
	double epsYd;
	double es;
};

struct Materiais
{
	ParametrosConcreto concreto;
	ParametrosAcoPassivo acoPassivo;
};

struct AlturasDeformacao
{
	double hLN;
	double hEpsLimite;
	double hEpsUltimo;
	double xAlpha;
	double xEpsLimite;
	double xEpsUltimo;
};

struct EsforcosInternos
{
	double normalConcreto;
	double momentoConcreto;
	double normalAcoPassivo;
	double momentoAcoPassivo;
	double maxCompressao;
	double maxTracao;
};

struct SomatorioEsforcos
{
	double somatorioNormal;
	double somatorioMomento;
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
	vector<double> eps1;
	vector<double> normal;
	vector<double> momento;
};

void renderizacaoPoligonal(vector<Ponto> vertices, string nomeVertices, string nomePoligonal);
void renderizacaoBarras(vector<Ponto> vertices, string nomeVerticesBarras);
void exibirDadosSecaoComposta(Secao secaoComposta);
vector<Ponto> adicionaVertices(vector<Ponto> verticesColetados);
Ponto calculaIntersecao(Ponto p1, Ponto p2, double yCorte);
VetoresAcimaAbaixo corteLinha(vector<Ponto> vertices, double yCorte);
double yMaximo(vector<Ponto> verticesPoligonal);
double yMinimo(vector<Ponto> verticesPoligonal);
double calcularArea(vector<Ponto> verticesPoligonal, vector<Ponto> verticesPoligonalVazada);
Ponto calcularCentroide(vector<Ponto> pVertices, vector<Ponto> pVazados);
vector<Ponto> transladarParaCG(vector<Ponto> vertices, Ponto cg);
ParametrosConcreto calculaParametrosConcreto(double fck, double gamaC);
ParametrosAcoPassivo calculaParametrosAcoPassivo(double fyk, double gamaY, double esY);
AlturasDeformacao calculaAlturasDeformacao(Materiais materiais, Poligonal poligono, double eps1, double eps2);
double normalConcretoRetangulo(Materiais materiais, double c1, double c2, double y);
double momentoConcretoRetangulo(Materiais materiais, double c1, double c2, double y);
double normalConcretoParabola(Materiais materiais, AlturasDeformacao alturas, double c1, double c2, double y);
double momentoConcretoParabola(Materiais materiais, AlturasDeformacao alturas, double c1, double c2, double y);
double forcaNormalConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas);
double calcularMomentoConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas);
double tensaoAcoPassivo(Materiais materiais, double epi);
double forcaNormalAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, double eps1, double eps2);
double calcularMomentoAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, double eps1, double eps2);
double forcaMaximaCompressao(Materiais materiais, Poligonal secao, Reforco reforco);
double forcaMaximaTracao(Materiais materiais, Reforco reforco);
double equilibrioNormalSecao(double normalConcreto, double normalAcoPassivo, double Nsd);
double equilibrioMomentoSecao(double normalConcreto, double momentoAcoPassivo, double Msd);
GraficoEsfxDef graficoEsforcoDeformacao(Secao secaoComposta, double eps2);
Materiais construtorFck30CA50();
Secao construtorSecao(vector<Ponto> vertices, vector<Ponto> vVazados, vector<Ponto> vBarras,
	vector<double> dBarras, double Nsd, double eps1, double eps2);
Secao construtorTriangulo(double Nsd, double eps1, double eps2);
Secao construtorSecaoT(double Nsd, double eps1, double eps2);
double calcularSomatorioNormal(Materiais materiais, Poligonal secao, Reforco armadura, double epsA, double epsB, double Nsd);
double calcularSomatorioMomento(Materiais materiais, Poligonal secao, Reforco armadura, double epsA, double epsB);

int main()
{
	// Comparação conjunto de deformações e esforço normal
	double Nsd = 100;
	double eps1 = -0.49159; // -3.5 N1600 // -3.4397 N2000 // -0.49159 N+100
	double eps2 = 10.552; //  N1600 // -0.080436 N2000 // 10.552 N+100
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
	vector<Ponto> vColetados = { {7.5,0}, {10,30}, {20, 40}, {20, 50}, {-20, 50}, {-20, 40}, {-10,30}, {-7.5,0} };
	vector<Ponto> vVazados = {};
	vector<Ponto> vBarras = { {5,2.5}, {5,7.5}, {-5, 7.5}, {-5,2.5} };
	vector<double> dBarras = { 1, 1, 1, 1 }; // em cm

	// Declaração e inicialização da poligonal e reforco
	Ponto cg = calcularCentroide(vColetados, vVazados);
	secao.verticesPoligonal = transladarParaCG(vColetados, cg);
	secao.verticesPoligonalVazada = transladarParaCG(vVazados, cg);
	secao.yMaximo = yMaximo(secao.verticesPoligonal);
	secao.yMinimo = yMinimo(secao.verticesPoligonal);
	secao.hSecao = secao.yMaximo - secao.yMinimo;
	secao.area = calcularArea(secao.verticesPoligonal, secao.verticesPoligonalVazada);
	armadura.barras = transladarParaCG(vBarras, cg);
	armadura.diametros = dBarras;

	// Compressão e tração máximas
	esforcos.maxCompressao = forcaMaximaCompressao(materiais, secao, armadura);
	esforcos.maxTracao = forcaMaximaTracao(materiais, armadura);

	if (Nsd >= esforcos.maxCompressao && Nsd <= esforcos.maxTracao)
	{
		cout << "\nNsd dentro do limite: "
			<< esforcos.maxCompressao << " <= " << Nsd << " <= " << esforcos.maxTracao << endl;

		// =================== Verificação somatorio entre a regiao 1-2 ===================
		// Determinação das deformações da curvatura limite entre a região 1-2
		double eps1 = -(materiais.concreto.epsUltimo);
		double eps2 = 0;
		double somatorioNormal12 = calcularSomatorioNormal(materiais, secao, armadura, eps1, eps2, Nsd);

		cout << "\n--------------------------------------------" << endl;
		cout << "Regiao 1-2" << endl;
		cout << "eps1: " << eps1 << endl;
		cout << "eps2: " << eps2 << endl;
		cout << "sN1-2: " << somatorioNormal12 << endl;


		// =================== Verificação somatorio regiao 2-3 ===================
		// Calculo da dimensao d 
		double yBarrasMin = 0;
		for (size_t i = 0; i < armadura.barras.size(); i++)
		{
			if (armadura.barras[i].y <= yBarrasMin)
				yBarrasMin = armadura.barras[i].y;
		}

		double d = secao.yMaximo - yBarrasMin;
		// Determinação das deformações da curvatura limite entre a região 2-3 
		// eps2 = (eps(Min ap.y) - eps1) * (hSecao/d) + eps1;
		eps1 = -(materiais.concreto.epsUltimo);
		eps2 = ((10 - eps1) * (secao.hSecao / d)) + eps1;
		double somatorioNormal23 = calcularSomatorioNormal(materiais, secao, armadura, eps1, eps2, Nsd);

		cout << "\n--------------------------------------------" << endl;
		cout << "Regiao 2-3" << endl;
		cout << "d: " << d << endl;
		cout << "eps1: " << eps1 << endl;
		cout << "eps2: " << eps2 << endl;
		cout << "sN2-3: " << somatorioNormal23 << endl;


		// =================== Aplicação do método numérico da falsa posição ===================
		int contIteracoes = 0;
		int limiteIteracoes = 100;
		double precisaoSoma = 0.001;
		double somatorioNormal = precisaoSoma + 1;
		double momentoResistente = 0;

		if (somatorioNormal23 <= 0)
		{
			// falsa posição da região III
			// fixa-se eps2 = ((10 - 3.5) * (secao.hSecao / d)) + 3.5;
			// e varia eps1

			// Calculo da dimensao d 
			double yBarrasMin = 0;
			for (size_t i = 0; i < armadura.barras.size(); i++)
			{
				if (armadura.barras[i].y <= yBarrasMin)
					yBarrasMin = armadura.barras[i].y;
			}

			double d = secao.yMaximo - yBarrasMin;

			double epcu = -(materiais.concreto.epsUltimo);
			double epsu = 10;

			// -3.5 <= epsA <= 10
			// 10 <= epsB <= ((epsu - epcu) * (secao.hSecao / d)) + epcu;

			double epsAminimo = epcu;
			double epsBminimo = ((epsu - epsAminimo) * (secao.hSecao / d)) + epsAminimo;
			double somatorioMinimo = calcularSomatorioNormal(materiais, secao, armadura, epsAminimo, epsBminimo, Nsd);

			double epsAmaximo = epsu;
			double epsBmaximo = ((epsu - epsAmaximo) * (secao.hSecao / d)) + epsAmaximo;
			double somatorioMaximo = calcularSomatorioNormal(materiais, secao, armadura, epsAmaximo, epsBminimo, Nsd);

			double ak = epsAminimo;
			double epsAak = epsAminimo;
			double epsBak = epsBminimo;
			double fak = somatorioMinimo;

			double bk = epsAmaximo;
			double epsAbk = epsAmaximo;
			double epsBbk = epsBmaximo;
			double fbk = somatorioMaximo;

			double ck = 0;
			double epsAck = 0;
			double epsBck = 0;
			double fck = 0;

			bool valido = (fak * fbk < 0);

			if (!valido)
				cout << "Erro: Metodo da falsa posicao requer fak*fbk < 0 para garantir uma raiz no intervalo";

			while (valido && contIteracoes < limiteIteracoes && abs(somatorioNormal) > precisaoSoma)
			{
				contIteracoes++;

				ck = bk - ((fbk * (bk - ak)) / (fbk - fak));
				epsAck = ck;
				epsBck = ((epsu - epsAck) * (secao.hSecao / d)) + epsAck;
				fck = calcularSomatorioNormal(materiais, secao, armadura, epsAck, epsBck, Nsd);

				bool verificaSinal = (fak * fck > 0);

				if (verificaSinal)
				{
					ak = ck;
					epsAak = ak;
					epsBak = ((epsu - epsAak) * (secao.hSecao / d)) + epsAak;
					fak = calcularSomatorioNormal(materiais, secao, armadura, epsAak, epsBak, Nsd);
				}
				else
				{
					bk = ck;
					epsAbk = bk;
					epsBbk = ((epsu - epsAbk) * (secao.hSecao / d)) + epsAbk;
					fbk = calcularSomatorioNormal(materiais, secao, armadura, epsAbk, epsBbk, Nsd);
				}

				somatorioNormal = fck;
			}

			//// Cálculo do momento resistente
			epsAck = ck;
			epsBck = ((epsu - epsAck) * (secao.hSecao / d)) + epsAck;
			momentoResistente = calcularSomatorioMomento(materiais, secao, armadura, epsAck, epsBck);

			cout << "\n------------------RESULTADOS------------------" << endl;
			cout << "contIter: " << contIteracoes << endl;
			cout << "epsA: " << epsAck << endl;
			cout << "epsB: " << epsBck << endl;
			cout << "Mrd: " << momentoResistente << endl;

			//// Inicializa as coordenadas do polo da regiao I
			//double epcu = -(materiais.concreto.epsUltimo);
			//double epsu = 10;
			//double epsB = ((10 - epcu) * (secao.hSecao / d)) + epcu;
			//double xpRegiaoIII = d;
			//double ypRegiaoIII = epsu; // epsu

			//// Inicializa as variáveis do método numérico
			//double ak = 0;
			//double epsAak = ypRegiaoIII + ak * (-xpRegiaoIII);
			//double epsBak = ypRegiaoIII + ak * (secao.hSecao - xpRegiaoIII);
			//double fak = calcularSomatorioNormal(materiais, secao, armadura, epsAak, epsBak, Nsd);

			//double bk = (epsB - (-materiais.concreto.epsUltimo)) / secao.hSecao;
			//double epsAbk = ypRegiaoIII + bk * (-xpRegiaoIII);
			//double epsBbk = ypRegiaoIII + bk * (secao.hSecao - xpRegiaoIII);
			//double fbk = calcularSomatorioNormal(materiais, secao, armadura, epsAbk, epsBbk, Nsd);

			//double ck = 0;
			//double epsAck = 0;
			//double epsBck = 0;

			//cout << "\n--------------------------------------------" << endl;
			//cout << "Regiao III" << endl;
			//cout << "\nak: " << ak << endl;
			//cout << "epsAak: " << epsAak << endl;
			//cout << "epsBak: " << epsBak << endl;
			//cout << "fak: " << fak << endl;
			//cout << "\nbk: " << bk << endl;
			//cout << "epsAbk: " << epsAbk << endl;
			//cout << "epsBbk: " << epsBbk << endl;
			//cout << "fbk: " << fbk << endl;
			//cout << endl;

			//// variavel booleana para verificar se há uma raiz entre o intervalo de deformações
			//bool valido = (fak * fbk < 0);

			//if (!valido)
			//	cout << "Erro: Metodo da falsa posicao requer fak*fbk < 0 para garantir uma raiz no intervalo";

			//while (valido && contIteracoes < limiteIteracoes && abs(somatorioNormal) > precisaoSoma)
			//{
			//	contIteracoes++;

			//	ck = bk - ((fbk * (bk - ak)) / (fbk - fak));
			//	epsAck = ypRegiaoIII + ck * (-xpRegiaoIII);
			//	epsBck = ypRegiaoIII + ck * (secao.hSecao - xpRegiaoIII);
			//	double fck = calcularSomatorioNormal(materiais, secao, armadura, epsAck, epsBck, Nsd);

			//	bool verificaSinal = (fak * fck > 0);

			//	if (verificaSinal)
			//	{
			//		ak = ck;
			//		epsAak = ypRegiaoIII + ak * (-xpRegiaoIII);
			//		epsBak = ypRegiaoIII + ak * (secao.hSecao - xpRegiaoIII);
			//		fak = calcularSomatorioNormal(materiais, secao, armadura, epsAak, epsBak, Nsd);
			//	}
			//	else
			//	{
			//		bk = ck;
			//		epsAbk = ypRegiaoIII + bk * (-xpRegiaoIII);
			//		epsBbk = ypRegiaoIII + bk * (secao.hSecao - xpRegiaoIII);
			//		fbk = calcularSomatorioNormal(materiais, secao, armadura, epsAbk, epsBbk, Nsd);
			//	}

			//	somatorioNormal = fck;

			//	cout << "\nsN: " << somatorioNormal << endl;
			//	cout << "\nak: " << ak << endl;
			//	cout << "epsAak: " << epsAak << endl;
			//	cout << "epsBak: " << epsBak << endl;
			//	cout << "fak: " << fak << endl;
			//	cout << "\nbk: " << bk << endl;
			//	cout << "epsAbk: " << epsAbk << endl;
			//	cout << "epsBbk: " << epsBbk << endl;
			//	cout << "fbk: " << fbk << endl;
			//	cout << "\nck: " << ck << endl;
			//	cout << "epsAck: " << epsAck << endl;
			//	cout << "epsBck: " << epsBck << endl;
			//	cout << "fck: " << fck << endl;
			//	cout << endl;
			//}

			//// Cálculo do momento resistente
			//epsAck = ypRegiaoIII + ck * (-xpRegiaoIII);
			//epsBck = ypRegiaoIII + ck * (secao.hSecao - xpRegiaoIII);
			//momentoResistente = calcularSomatorioMomento(materiais, secao, armadura, epsAck, epsBck);

			//cout << "\n------------------RESULTADOS------------------" << endl;
			//cout << "contIter: " << contIteracoes << endl;
			//cout << "epsA: " << epsAck << endl;
			//cout << "epsB: " << epsBck << endl;
			//cout << "Mrd: " << momentoResistente << endl;

		}
		else if (somatorioNormal12 <= 0)
		{
			// falsa posição da região II
			// fixa-se eps1 = -3.5
			// e varia eps2 entre eps2 = ((10 - 3.5) * (secao.hSecao / d)) + 3.5 e eps = 0

			// Inicializa as coordenadas do polo da regiao I
			double epsA = -(materiais.concreto.epsUltimo);

			// Inicializa as variáveis do método numérico
			double ak = 0;
			double bk = 10;
			double ck = 0;

			double fak = calcularSomatorioNormal(materiais, secao, armadura, epsA, ak, Nsd);
			double fbk = calcularSomatorioNormal(materiais, secao, armadura, epsA, bk, Nsd);

			cout << "\n--------------------------------------------" << endl;
			cout << "Regiao II" << endl;
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

				double fck = calcularSomatorioNormal(materiais, secao, armadura, epsA, ck, Nsd);

				bool verificaSinal = (fak * fck > 0);

				if (verificaSinal)
				{
					ak = ck;
					fak = calcularSomatorioNormal(materiais, secao, armadura, epsA, ak, Nsd);
				}
				else
				{
					bk = ck;
					fbk = calcularSomatorioNormal(materiais, secao, armadura, epsA, bk, Nsd);
				}

				somatorioNormal = fck;

				cout << "\nsN: " << somatorioNormal << endl;
				cout << "ak: " << ak << endl;
				cout << "bk: " << bk << endl;
				cout << "ck: " << ck << endl;
			}

			momentoResistente = calcularSomatorioMomento(materiais, secao, armadura, epsA, ck);

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

			// Regiao I, o polo de ruina B está sempre encurtado em 2 por mil
			// y - yp = a(x-xp)
			// xp,yp (3*h/7, -2)
			// y1 = yp + a(x-150/7) = -2 + a(-150/7)
			// y2 = yp + a(x-150/7) = -2 + a(50 - 150/7)

			// a0 = 0 - (-3.5) / 50 = 0.07
			// a1 = -2 - (-2) / 50 = 0 

			// Inicializa as coordenadas do polo da regiao I
			double epcu = -(materiais.concreto.epsUltimo);
			double epc2 = -(materiais.concreto.epsLimite);
			double xpRegiaoI = secao.hSecao * ((epcu - epc2) / (epcu));
			double ypRegiaoI = epc2;

			// Inicializa as variáveis do método numérico
			double ak = (epc2 - epc2) / secao.hSecao;
			double epsAak = ypRegiaoI + ak * (-xpRegiaoI);
			double epsBak = ypRegiaoI + ak * (secao.hSecao - xpRegiaoI);
			double fak = calcularSomatorioNormal(materiais, secao, armadura, epsAak, epsBak, Nsd);

			double bk = (-epcu) / secao.hSecao;
			double epsAbk = ypRegiaoI + bk * (-xpRegiaoI);
			double epsBbk = ypRegiaoI + bk * (secao.hSecao - xpRegiaoI);
			double fbk = calcularSomatorioNormal(materiais, secao, armadura, epsAbk, epsBbk, Nsd);

			double ck = 0;
			double epsAck = 0;
			double epsBck = 0;

			cout << "\n--------------------------------------------" << endl;
			cout << "Regiao I" << endl;
			cout << "\nak: " << ak << endl;
			cout << "epsAak: " << epsAak << endl;
			cout << "epsBak: " << epsBak << endl;
			cout << "fak: " << fak << endl;
			cout << "\nbk: " << bk << endl;
			cout << "epsAbk: " << epsAbk << endl;
			cout << "epsBbk: " << epsBbk << endl;
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
				epsAck = ypRegiaoI + ck * (-xpRegiaoI);
				epsBck = ypRegiaoI + ck * (secao.hSecao - xpRegiaoI);
				double fck = calcularSomatorioNormal(materiais, secao, armadura, epsAck, epsBck, Nsd);

				// Verifica o sinal e ajusta os limites
				if (fak * fck > 0)
				{
					ak = ck;
					epsAak = ypRegiaoI + ak * (-xpRegiaoI);
					epsBak = ypRegiaoI + ak * (secao.hSecao - xpRegiaoI);
					fak = calcularSomatorioNormal(materiais, secao, armadura, epsAak, epsBak, Nsd);
				}
				else
				{
					bk = ck;
					epsAbk = ypRegiaoI + bk * (-xpRegiaoI);
					epsBbk = ypRegiaoI + bk * (secao.hSecao - xpRegiaoI);
					fbk = calcularSomatorioNormal(materiais, secao, armadura, epsAbk, epsBbk, Nsd);
				}

				somatorioNormal = fck;

				cout << "\nsN: " << somatorioNormal << endl;
				cout << "\nak: " << ak << endl;
				cout << "epsAak: " << epsAak << endl;
				cout << "epsBak: " << epsBbk << endl;
				cout << "fak: " << fak << endl;
				cout << "\nbk: " << bk << endl;
				cout << "epsAbk: " << epsAbk << endl;
				cout << "epsBbk: " << epsBbk << endl;
				cout << "fbk: " << fbk << endl;
				cout << "\nck: " << ck << endl;
				cout << "epsAck: " << epsAck << endl;
				cout << "epsBck: " << epsBck << endl;
				cout << "fck: " << fck << endl;
				cout << endl;
			}

			// Cálculo do momento resistente
			epsAck = ypRegiaoI + ck * (-xpRegiaoI);
			epsBck = ypRegiaoI + ck * (secao.hSecao - xpRegiaoI);
			momentoResistente = calcularSomatorioMomento(materiais, secao, armadura, epsAck, epsBck);

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
		// inicialização do tipo vector<double> nome(tamanho)
		vector<double> xTemp(vertices.size());
		vector<double> yTemp(vertices.size());

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
			vector<double> xTempAresta(vertices.size() + 1);
			vector<double> yTempAresta(vertices.size() + 1);

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
		// inicialização do tipo vector<double> nome(tamanho)
		vector<double> xTemp(vertices.size());
		vector<double> yTemp(vertices.size());

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
		// variáveis double para compor a struct Ponto
		double pontoXi = verticesColetados[i].x;
		double pontoYi = verticesColetados[i].y;

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
Ponto calculaIntersecao(Ponto p1, Ponto p2, double yCorte) {
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	double t = (yCorte - p1.y) / dy;  // Interpolação linear
	return { p1.x + t * dx, yCorte };
}

// Função para cortar a seção transversal
VetoresAcimaAbaixo corteLinha(vector<Ponto> vertices, double yCorte)
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

double yMaximo(vector<Ponto> verticesPoligonal)
{
	if (verticesPoligonal.empty())
		throw invalid_argument("O vetor de vertices está vazio.");

	double yMax = verticesPoligonal[0].y;

	for (size_t i = 0; i < verticesPoligonal.size(); i++)
	{
		if (verticesPoligonal[i].y > yMax) yMax = verticesPoligonal[i].y;
	}

	return yMax;
}

double yMinimo(vector<Ponto> verticesPoligonal)
{
	if (verticesPoligonal.empty())
		throw invalid_argument("O vetor de vertices está vazio.");

	double yMin = verticesPoligonal[0].y;

	for (size_t i = 0; i < verticesPoligonal.size(); i++)
	{
		if (verticesPoligonal[i].y < yMin) yMin = verticesPoligonal[i].y;
	}

	return yMin;
}

double calcularArea(vector<Ponto> verticesPoligonal, vector<Ponto> verticesPoligonalVazada)
{
	double area = 0;
	double areaExterna = 0;
	double areaInterna = 0;

	int nExterna = verticesPoligonal.size();
	for (int i = 0; i < nExterna; i++)
	{
		int j = (i + 1) % nExterna;
		areaExterna += verticesPoligonal[i].x * verticesPoligonal[j].y;
		areaExterna -= verticesPoligonal[j].x * verticesPoligonal[i].y;
	}

	areaExterna = abs(areaExterna) / 2; // cm^2

	if (verticesPoligonalVazada.empty())
	{
		return areaExterna / 10000; // m^2
	}

	int nInterna = verticesPoligonalVazada.size();
	for (int i = 0; i < nInterna; i++)
	{
		int j = (i + 1) % nInterna;
		areaInterna += verticesPoligonalVazada[i].x * verticesPoligonalVazada[j].y;
		areaInterna += verticesPoligonalVazada[j].x * verticesPoligonalVazada[i].x;
	}

	areaInterna = abs(areaInterna) / 2; // cm^2

	area = (areaExterna - areaInterna) / 10000; // m^2

	return area; // m^2
}

Ponto calcularCentroide(vector<Ponto> pVertices, vector<Ponto> pVazados)
{
	double areaExterna = 0;
	double estaticaXexterna = 0;
	double estaticaYexterna = 0;

	int nExterna = pVertices.size();
	for (int i = 0; i < nExterna; i++)
	{
		int j = (i + 1) % nExterna;

		double fator = (pVertices[i].x * pVertices[j].y) - (pVertices[j].x * pVertices[i].y);

		areaExterna += fator;

		estaticaXexterna += (pVertices[i].y + pVertices[j].y) * fator;

		estaticaYexterna += (pVertices[i].x + pVertices[j].x) * fator;
	}

	areaExterna = abs(areaExterna) / 2;
	estaticaXexterna /= 6;
	estaticaYexterna /= 6;

	double xCGexterna = estaticaYexterna / areaExterna;
	double yCGexterna = estaticaXexterna / areaExterna;

	if (pVazados.empty())
	{
		return Ponto(xCGexterna, yCGexterna);
	}

	double areaInterna = 0;
	double estaticaXinterna = 0;
	double estaticaYinterna = 0;

	int nInterna = pVazados.size();
	for (int i = 0; i < nInterna; i++)
	{
		int j = (i + 1) % nInterna;

		double fator = (pVazados[i].x * pVazados[j].y) - (pVazados[j].x * pVazados[i].y);

		areaInterna += fator;

		estaticaXinterna += (pVazados[i].y + pVazados[j].y) * fator;

		estaticaYinterna += (pVazados[i].x + pVazados[j].x) * fator;
	}

	areaInterna = abs(areaInterna) / 2;
	estaticaXinterna /= 6;
	estaticaYinterna /= 6;

	double xCGinterna = estaticaYinterna / areaInterna;
	double yCGinterna = estaticaXinterna / areaInterna;

	double xCG = (xCGexterna * areaExterna - xCGinterna * areaInterna) / (areaExterna - areaInterna);
	double yCG = (yCGexterna * areaExterna - xCGinterna * areaInterna) / (areaExterna - areaInterna);

	return Ponto(xCG, yCG);
}

vector<Ponto> transladarParaCG(vector<Ponto> vertices, Ponto cg)
{
	vector<Ponto> verticesTemp;

	for (size_t i = 0; i < vertices.size(); i++)
	{
		// variáveis double para compor a struct Ponto
		double pontoXi = vertices[i].x - cg.x;
		double pontoYi = vertices[i].y - cg.y;

		// alimentação do vetor temporário às coordenadas temporárias criadas
		verticesTemp.push_back(Ponto(pontoXi, pontoYi));
	}

	return verticesTemp;
}

ParametrosConcreto calculaParametrosConcreto(double fck, double gamaC)
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

ParametrosAcoPassivo calculaParametrosAcoPassivo(double fyk, double gamaY, double esY)
{
	ParametrosAcoPassivo parametros;

	parametros.fyd = fyk / gamaY; // MPa
	parametros.es = esY;
	// Deformação por mil; MPa / GPa
	parametros.epsYd = parametros.fyd / (parametros.es);

	return parametros;
}

AlturasDeformacao calculaAlturasDeformacao(Materiais materiais, Poligonal poligono, double eps1, double eps2)
{
	AlturasDeformacao alturasTemp;
	double k = poligono.hSecao / ((eps2 / 1000) - (eps1 / 1000));

	alturasTemp.xAlpha = (-eps1 / 1000) * k;
	alturasTemp.xEpsLimite = ((-materiais.concreto.epsLimite / 1000) - (eps1 / 1000)) * k;
	alturasTemp.xEpsUltimo = ((-materiais.concreto.epsUltimo / 1000) - (eps1 / 1000)) * k;
	alturasTemp.hLN = poligono.yMaximo - alturasTemp.xAlpha;
	alturasTemp.hEpsLimite = poligono.yMaximo - alturasTemp.xEpsLimite;
	alturasTemp.hEpsUltimo = poligono.yMaximo - alturasTemp.xEpsUltimo;

	return alturasTemp;
}

double normalConcretoRetangulo(Materiais materiais, double c1, double c2, double y)
{
	double nctr = materiais.concreto.multFcd * materiais.concreto.fcd *
		(c1 * y + c2 * y * y / 2);

	return nctr * 1000; // kN
}

double momentoConcretoRetangulo(Materiais materiais, double c1, double c2, double y)
{
	double mctr = materiais.concreto.multFcd * materiais.concreto.fcd *
		(c1 * y * y / 2 + c2 * y * y * y / 3);

	return mctr * 1000; // kN
}

double normalConcretoParabola(Materiais materiais, AlturasDeformacao alturas, double c1, double c2, double y)
{
	double g = alturas.hLN / 100; // m
	double xEpc2 = (alturas.hEpsLimite - alturas.hLN) / 100; // m
	double n1 = materiais.concreto.nConc + 1;
	double n2 = materiais.concreto.nConc + 2;
	double n3 = materiais.concreto.nConc + 3;
	double fcd = materiais.concreto.fcd; // MPa
	double multFcd = materiais.concreto.multFcd;
	double nConc = materiais.concreto.nConc;
	double eexp = pow(((g + xEpc2 - y) / xEpc2), n1);

	double nctp = -multFcd * fcd *
		(-((xEpc2 * eexp * (c1 * n2 + c2 * (g + xEpc2 + nConc * y + y))) / (n1 * n2)) -
			c1 * y - (c2 * y * y) / 2);

	return nctp * 1000;
}

double momentoConcretoParabola(Materiais materiais, AlturasDeformacao alturas, double c1, double c2, double y)
{
	double g = alturas.hLN / 100; // m
	double xEpc2 = (alturas.hEpsLimite - alturas.hLN) / 100; // m
	double n1 = materiais.concreto.nConc + 1;
	double n2 = materiais.concreto.nConc + 2;
	double n3 = materiais.concreto.nConc + 3;
	double fcd = materiais.concreto.fcd;
	double multFcd = materiais.concreto.multFcd;
	double nConc = materiais.concreto.nConc;
	double eexp = pow(((g + xEpc2 - y) / xEpc2), n1);

	double mctp = (multFcd * fcd * (3 * c1 * ((n1) * (n2) * (n3)*y * y + 2 * xEpc2 * eexp * (((g)
		+xEpc2) * (n3)+(3 + 4 * nConc + nConc * nConc) * y)) + 2 * c2 * ((n1) * (n2) * (n3)
			*y * y * y + 3 * xEpc2 * eexp * (2 * (g) * (g)+2 * xEpc2 * xEpc2 + 2 *
				xEpc2 * (n1)*y + (2 + 3 * nConc + nConc * nConc) * y * y + 2 * (g) * (2 *
					xEpc2 + (n1)*y))))) / (6 * (n1) * (n2) * (n3));

	return mctp * 1000;
}

double forcaNormalConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas)
{
	vector<Ponto> vetTempAcima = pComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = pComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	double NC = 0;
	double NCTP = 0;
	double NCTR = 0;

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		int j = i + 1;
		double x1 = vetTempAbaixo[i].x;
		double y1 = vetTempAbaixo[i].y;
		double x2;
		double y2;

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
			double c1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
			double c2 = (x1 - x2) / (y1 - y2); // adimensional

			double nc1 = normalConcretoParabola(materiais, alturas, c1, c2, y1);
			double nc2 = normalConcretoParabola(materiais, alturas, c1, c2, y2);

			NCTP = NCTP + nc2 - nc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		int j = i + 1;
		double x1 = vetTempAcima[i].x;
		double y1 = vetTempAcima[i].y;
		double x2;
		double y2;

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
			double c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			double c2 = (x1 - x2) / (y1 - y2);

			double nc1 = normalConcretoRetangulo(materiais, c1, c2, y1);
			double nc2 = normalConcretoRetangulo(materiais, c1, c2, y2);

			NCTR = NCTR + nc2 - nc1;
		}
	}

	return NC = -(NCTP + NCTR);
}

double calcularMomentoConcreto(Materiais materiais, Poligonal pComprimido, AlturasDeformacao alturas)
{
	vector<Ponto> vetTempAcima = pComprimido.vetoresAcimaAbaixo.verticesAcima;
	vector<Ponto> vetTempAbaixo = pComprimido.vetoresAcimaAbaixo.verticesAbaixo;

	double MC = 0;
	double MCTR = 0;
	double MCTP = 0;

	for (int i = 0; i < vetTempAbaixo.size(); i++)
	{
		int j = i + 1;
		double x1 = vetTempAbaixo[i].x;
		double y1 = vetTempAbaixo[i].y;
		double x2;
		double y2;

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
			double c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			double c2 = (x1 - x2) / (y1 - y2);

			double mc1 = momentoConcretoParabola(materiais, alturas, c1, c2, y1);
			double mc2 = momentoConcretoParabola(materiais, alturas, c1, c2, y2);

			MCTP = MCTP + mc2 - mc1;
		}
	}

	for (int i = 0; i < vetTempAcima.size(); i++)
	{
		int j = i + 1;
		double x1 = vetTempAcima[i].x;
		double y1 = vetTempAcima[i].y;
		double x2;
		double y2;

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
			double c1 = (y1 * x2 - y2 * x1) / (y1 - y2);
			double c2 = (x1 - x2) / (y1 - y2);

			double mc1 = momentoConcretoRetangulo(materiais, c1, c2, y1);
			double mc2 = momentoConcretoRetangulo(materiais, c1, c2, y2);

			MCTR = MCTR + mc2 - mc1;
		}
	}

	return MC = MCTP + MCTR;
}

double tensaoAcoPassivo(Materiais materiais, double epi)
{
	double tensaoAcoPassivo;

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

double forcaNormalAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, double eps1, double eps2)
{
	double NY = 0;

	double k = (eps2 - eps1) / secao.hSecao;

	double deformacaoBarra = 0;

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

		double tensao = tensaoAcoPassivo(materiais, deformacaoBarra); // MPa

		double areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		double ny = tensao * areaBarra / 10; // kN

		NY += ny; // kN
	}

	return NY;
}

double calcularMomentoAcoPassivo(Materiais materiais, Poligonal secao, Reforco reforco, double eps1, double eps2)
{
	double MY = 0;

	double k = (eps2 - eps1) / secao.hSecao;

	double deformacaoBarra = 0;

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

		double tensao = tensaoAcoPassivo(materiais, deformacaoBarra);

		double areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4;

		double ny = tensao * areaBarra / 10; // kN
		double my = -ny * (reforco.barras[i].y / 100); // kN*cm^2/100 = kN*m

		MY += my; // kN*m
	}

	return MY;
}

double forcaMaximaCompressao(Materiais materiais, Poligonal secao, Reforco reforco)
{
	double NMIN = 0;
	double Nap = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		double tensao = tensaoAcoPassivo(materiais, -(materiais.concreto.epsLimite));

		double areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		double nap = tensao * areaBarra / 10; // kN

		Nap += nap;
	}

	return NMIN = -(secao.area * materiais.concreto.fcd * materiais.concreto.multFcd) * 1000 + Nap;
}

double forcaMaximaTracao(Materiais materiais, Reforco reforco)
{
	double NMAX = 0;
	double Nap = 0;

	for (size_t i = 0; i < reforco.barras.size(); i++)
	{
		double tensao = tensaoAcoPassivo(materiais, 10);

		double areaBarra = pow(reforco.diametros[i], 2) * M_PI / 4; // cm^2

		double nap = tensao * areaBarra / 10; // kN

		Nap += nap;
	}

	return NMAX = Nap;
}

double equilibrioNormalSecao(double normalConcreto, double normalAcoPassivo, double Nsd)
{
	return normalConcreto + normalAcoPassivo - Nsd;
}

double equilibrioMomentoSecao(double momentoConcreto, double momentoAcoPassivo, double Msd)
{
	return momentoConcreto + momentoAcoPassivo - Msd;
}

GraficoEsfxDef graficoEsforcoDeformacao(Secao secaoComposta, double eps2)
{
	GraficoEsfxDef temp;

	const int tamanho = 36;
	vector<double> EPS1(tamanho);
	vector<double> normal(tamanho);
	vector<double> momento(tamanho);

	for (int i = 0; i < tamanho; i++)
		EPS1[i] = -(0 + i * 0.1f);

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
	double fck = 30; //MPa
	double gamaC = 1.4;
	ParametrosConcreto concreto = calculaParametrosConcreto(fck, gamaC);

	double fyk = 500; // MPa
	double gamaS = 1.15;
	double es = 210; // GPa
	ParametrosAcoPassivo acoPassivo = calculaParametrosAcoPassivo(fyk, gamaS, es);

	Materiais temp = { concreto, acoPassivo };

	return temp;
}

Secao construtorSecao(vector<Ponto> vertices, vector<Ponto> vVazados, vector<Ponto> vBarras,
	vector<double> dBarras, double Nsd, double eps1, double eps2)
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
	poligono.area = calcularArea(poligono.verticesPoligonal, poligono.verticesPoligonalVazada); // m^2
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
		poligonoComprimido.area = calcularArea(poligonoComprimido.verticesPoligonal, poligonoComprimido.verticesPoligonalVazada);
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

Secao construtorTriangulo(double Nsd, double eps1, double eps2)
{
	// Declaração e inicialização dos vértices
	vector<Ponto> vColetados = { {0,20}, {-10,-20}, {10,-20} };
	vector<Ponto> vVazados = {};
	vector<Ponto> vBarras = { {-7, -17}, {7, -17} };

	Ponto cg = calcularCentroide(vColetados, vVazados);

	vector<Ponto> pVertices = transladarParaCG(vColetados, cg);
	vector<Ponto> pVazados = transladarParaCG(vVazados, cg);
	vector<Ponto> pBarras = transladarParaCG(vBarras, cg);
	vector<double> dBarras = { 1, 1 }; // em cm

	Secao secaoComposta = construtorSecao(pVertices, pVazados, pBarras, dBarras, Nsd, eps1, eps2);

	return secaoComposta;
}

Secao construtorSecaoT(double Nsd, double eps1, double eps2)
{
	// Declaração e inicialização dos vértices
	vector<Ponto> vColetados = { {7.5,0}, {10,30}, {20, 40}, {20, 50}, {-20, 50}, {-20, 40}, {-10,30}, {-7.5,0} };
	vector<Ponto> vVazados = {};
	vector<Ponto> vBarras = { {5,2.5}, {5,7.5}, {-5, 7.5}, {-5,2.5} };

	Ponto cg = calcularCentroide(vColetados, vVazados);

	vector<Ponto> pVertices = transladarParaCG(vColetados, cg);
	vector<Ponto> pVazados = transladarParaCG(vVazados, cg);
	vector<Ponto> pBarras = transladarParaCG(vBarras, cg);
	vector<double> dBarras = { 1, 1, 1, 1 }; // em cm

	Secao secaoComposta = construtorSecao(pVertices, pVazados, pBarras, dBarras, Nsd, eps1, eps2);

	return secaoComposta;
}

double calcularSomatorioNormal(Materiais materiais, Poligonal secao, Reforco armadura, double epsA, double epsB, double Nsd)
{
	double somatorioNormal = 0;

	if (epsA == epsB)
	{
		if (epsA == -(materiais.concreto.epsLimite))
			return somatorioNormal = forcaMaximaCompressao(materiais, secao, armadura);

		if (epsA == 10)
			return somatorioNormal = forcaMaximaTracao(materiais, armadura);
	}

	// Determinação das alturas relativas à curvatura limite entre a região
	AlturasDeformacao alturas = calculaAlturasDeformacao(materiais, secao, epsA, epsB);

	// Divisão da secao entre as áreas comprimidas, comprimidas parabola e comprimidas retangulo da regiao
	Poligonal secaoComprimida;

	if (epsA < 0)
	{
		secao.vetoresAcimaAbaixo = corteLinha(secao.verticesPoligonal, alturas.hLN);
		secaoComprimida.verticesPoligonal = secao.vetoresAcimaAbaixo.verticesAcima;
		secaoComprimida.vetoresAcimaAbaixo = corteLinha(secaoComprimida.verticesPoligonal, alturas.hEpsLimite);
	}

	// Verificação do esforco normal e somatorio no limite entre a regiao
	double normalConcreto = forcaNormalConcreto(materiais, secaoComprimida, alturas);
	double normalAcoPassivo = forcaNormalAcoPassivo(materiais, secao, armadura, epsA, epsB);
	somatorioNormal = equilibrioNormalSecao(normalConcreto, normalAcoPassivo, Nsd);

	return somatorioNormal;
}

double calcularSomatorioMomento(Materiais materiais, Poligonal secao, Reforco armadura, double epsA, double epsB)
{
	// Determinação das alturas relativas à curvatura limite entre a região
	AlturasDeformacao alturas = calculaAlturasDeformacao(materiais, secao, epsA, epsB);

	// Divisão da secao entre as áreas comprimidas, comprimidas parabola e comprimidas retangulo da regiao
	Poligonal secaoComprimida;

	if (epsA < 0)
	{
		secao.vetoresAcimaAbaixo = corteLinha(secao.verticesPoligonal, alturas.hLN);
		secaoComprimida.verticesPoligonal = secao.vetoresAcimaAbaixo.verticesAcima;
		secaoComprimida.vetoresAcimaAbaixo = corteLinha(secaoComprimida.verticesPoligonal, alturas.hEpsLimite);
	}

	// Verificação do esforco normal e somatorio no limite entre a regiao
	double momentoConcreto = calcularMomentoConcreto(materiais, secaoComprimida, alturas);
	double momentoAcoPassivo = calcularMomentoAcoPassivo(materiais, secao, armadura, epsA, epsB);
	double somatorioMomento = equilibrioMomentoSecao(momentoConcreto, momentoAcoPassivo, 0);

	return somatorioMomento;
}