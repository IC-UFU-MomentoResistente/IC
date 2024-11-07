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
    Poligono poligono;
    void AdicionarBarra(float posX, float posY, float diametro);
    void RemoverBarra();
    void translacaoCG(const std::vector<Point>& points, Point cg);
    void RotacionarArmadura(double angulo);
};


#endif // REFORCO_H