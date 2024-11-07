#include <iostream>
#include <vector>
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


