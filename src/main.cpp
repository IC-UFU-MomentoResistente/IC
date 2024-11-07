#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <set>
#include "poligono.h"
#include "concreto.h"
#include "reforco.h"

int main() {
    // Exemplo de uso da classe Poligono
    std::vector<Point> collectedPoints = {
        {0,0}, {100,0}, {50,100}
    }; 

    Poligono poligono;
    float linhaDeCorte = 50.0;
    poligono.setVertices(collectedPoints);
    poligono.cortarPoligonal(collectedPoints, linhaDeCorte);

    std::cout << "Pontos após o corte:" << std::endl;
    for (const auto& p : poligono.resultadoCorte) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Comprimida:" << std::endl;
    for (const auto& p : poligono.areaSuperior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Tracionada:" << std::endl;
    for (const auto& p : poligono.areaInferior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    // Exemplo de uso da classe Concreto
    Concreto concreto(30.0, 1.4, 3.5, 2.0, 0.5, 100.0);
    Concreto::ParametrosConcreto parametros = concreto.getParametros();
    std::cout << "Fator multiplicador" << parametros.fatorMultTensaoCompConcreto << std::endl;
    std::cout << "Expoente tensão" << parametros.expoenteTensaoConcreto << std::endl;
    std::cout << "eps_2" << parametros.epsilonConcreto2 << std::endl;
    std::cout << "eps_ultimo" << parametros.epsilonConcretoUltimo << std::endl;
    std::cout << "Fcd: " << parametros.fcd << std::endl;

    // Exemplo de uso da classe Reforco
    Reforco reforco;
    reforco.AdicionarBarra(10.0, 20.0, 12.0);
    reforco.AdicionarBarra(15.0, 25.0, 16.0);
    std::cout << "\nBarras de Reforço Adicionadas:" << std::endl;
    for (size_t i = 0; i < reforco.Armaduras.size(); ++i) {
        std::cout << "Posição: (" << reforco.Armaduras[i].x << ", " << reforco.Armaduras[i].y << "), Diâmetro: " << reforco.valorDiametroBarras[i] << std::endl;
    }

    return 0;
}