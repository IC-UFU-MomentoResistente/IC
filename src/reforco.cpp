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
