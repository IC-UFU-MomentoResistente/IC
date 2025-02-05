#ifndef REFORCO_H
#define REFORCO_H

#include <vector>
#include <set>
#include <cmath>
#include "point.h"
#include "poligono.h"

#define _USE_MATH_DEFINES

class Reforco
{
public:
    std::vector<Point> Armaduras;               // Vetor para armazenar as barras
    std::vector<float> valorDiametroBarras;     // Vetor para armazenar os diâmetros
    std::vector<Point> barrasTransladadas; 
    std::vector<Point> barrasRotacionadas;
    std::vector<float> aco_passivo_normal;
    std::vector<float> aco_passivo_momento; 
    std::vector<float> area_barra; 
    std::vector<float> tensao_barra;
    std::vector<float> Epi; 

    Poligono poligono;
    float fyd;
    float epsilon_yd;
    float Es;
    float deformacao_barra;
    float tensao_aco_passivo;
    float area_barra_variavel;
    float normal_aco_passivo_variavel;
    float momento_aco_passivo_variavel;
    float soma_normal_aco_passivo;
    float soma_momento_aco_passivo;
    
    void AdicionarBarra(float posX, float posY, float diametro);
    void RemoverBarra();
    void translacaoCG(const std::vector<Point>& points, Point cg);
    void RotacionarArmadura(double angulo);
    void calculaParametros (float fyk, float gama_s, float Es_interface);
    float tensao (float epi);
    void calculaNormal_Momento(float Epap, float EpA);
    
};


#endif // REFORCO_H