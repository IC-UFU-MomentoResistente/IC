/* #define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include "cmath"
#include "poligono.h"
#include "reforco.h"
#include "point.h"

void Reforco::AdicionarBarra(float posX, float posY, float diametro)
{
    Armaduras.push_back(Point(posX, posY));  // Adiciona nova posição
    valorDiametroBarras.push_back(diametro); // Armazena o diâmetro
}

void Reforco::RemoverBarra()
{
    if (!Armaduras.empty())
    {
        Armaduras.pop_back();           // Remove a última barra
        valorDiametroBarras.pop_back(); // Remove o último diâmetro
    }
}

void Reforco::translacaoCG(const std::vector<Point>& points, Point cg)
{
    barrasTransladadas.clear();

    for(const auto& p : points){
        float u = p.x - cg.x; 
        float v = p.y - cg.y;
        barrasTransladadas.emplace_back(u,v);
    }
}

void Reforco::RotacionarArmadura(double angulo) {
    Armaduras.clear();
    double ang_radianos = angulo * M_PI / 180.0;

    for (const auto& p : barrasTransladadas) {
        float u = (p.x * cos(ang_radianos)) - (p.y * sin(ang_radianos));
        float v = (p.x * sin(ang_radianos)) + (p.y * cos(ang_radianos));
        Armaduras.emplace_back(u, v);
    }
} */


#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include "cmath"
#include "poligono.h"
#include "reforco.h"
#include "point.h"

void Reforco::AdicionarBarra(float posX, float posY, float diametro)
{
   
    Armaduras.push_back(Point(posX, posY));  // Adiciona nova posição
    valorDiametroBarras.push_back(diametro); // Armazena o diâmetro

    if(barrasRotacionadas.empty()) {
        barrasRotacionadas = Armaduras; 
    }

}

void Reforco::RemoverBarra()
{
    if (!Armaduras.empty())
    {
        Armaduras.pop_back();           // Remove a última barra
        valorDiametroBarras.pop_back(); // Remove o último diâmetro
    }
}

void Reforco::translacaoCG(const std::vector<Point>& points, Point cg)
{
    barrasTransladadas.clear();

    for(const auto& p : points){
        float u = p.x - cg.x; 
        float v = p.y - cg.y;
        barrasTransladadas.emplace_back(u, v);
    }
}

void Reforco::RotacionarArmadura(double angulo) {
    barrasRotacionadas.clear(); // Limpa o vetor de barras rotacionadas
    double ang_radianos = angulo * M_PI / 180.0;

    for (const auto& p : barrasTransladadas) {
        float u = (p.x * cos(ang_radianos)) - (p.y * sin(ang_radianos));
        float v = (p.x * sin(ang_radianos)) + (p.y * cos(ang_radianos));
        barrasRotacionadas.emplace_back(u, v); // Adiciona ponto rotacionado
    }
}

void Reforco::calculaParametros (float fyk, float gama_s, float Es){
   fyd = (fyk / gama_s);
   epsilon_yd = (fyd / (Es*1000));
}

void Reforco::tensao (float epi){
    if (epi>=epsilon_yd)
    {
        tensao_aco_passivo = fyd;
    }
    else if (epi <= -epsilon_yd) 
    {
        tensao_aco_passivo = -fyd;  
    }
    else if (epi > 10.001 || epi < -10.001) 
    {
        tensao_aco_passivo = 0;
    }
    else 
    {
        tensao_aco_passivo = (Es * epi/1000);
    }
}

void Reforco::calculaNormal_Momento(float Epap, float EpA) 
{
    soma_normal_aco_passivo = 0;
    soma_momento_aco_passivo = 0; 
    float yMinSecao = -20, yMaxSecao = 20;
    //poligono.MaxMin(yMinSecao, yMaxSecao);
    float yMinArmadura;

    for (const auto& p: Armaduras)
    {
        if (p.y < yMinArmadura) yMinArmadura = p.y;
    }
    
    float d = yMaxSecao - yMinArmadura;
    float k;

    //Epi[Armaduras.size()];
    //tensao_barra[Armaduras.size()];
    //area_barra[Armaduras.size()];
    //aco_passivo_momento[Armaduras.size()];
    //aco_passivo_normal[Armaduras.size()];
   
    for (size_t i = 0; i < Armaduras.size(); i++) 
    {   
        k = (Epap - EpA)/d;
        deformacao_barra = (k*(yMaxSecao - Armaduras[i].y))+EpA;
        Epi.push_back(deformacao_barra);
        tensao(Epi[i]);
        tensao_barra.push_back(tensao_aco_passivo);
        area_barra_variavel = (pow(valorDiametroBarras[i],2)*M_PI) / 4;
        area_barra.push_back(area_barra_variavel);
        normal_aco_passivo_variavel = tensao_aco_passivo*area_barra[i];
        momento_aco_passivo_variavel = aco_passivo_normal[i]* (Armaduras[i].y);    
        aco_passivo_normal.push_back(normal_aco_passivo_variavel);
        aco_passivo_momento.push_back(momento_aco_passivo_variavel);
        soma_normal_aco_passivo = soma_normal_aco_passivo + aco_passivo_normal[i];
        soma_momento_aco_passivo = soma_momento_aco_passivo + aco_passivo_momento[i];
    }
}