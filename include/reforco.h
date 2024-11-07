#ifndef REFORCO_H
#define REFORCO_H

#include <vector>
#include <set>
#include <cmath>
#include "point.h"

#define _USE_MATH_DEFINES

class Reforco
{
public:
    std::vector<Point> Armaduras;               // Vetor para armazenar as barras
    std::vector<float> valorDiametroBarras;     // Vetor para armazenar os diâmetros

    void AdicionarBarra(float posX, float posY, float diametro);
    void RemoverBarra();
};


#endif // REFORCO_H