#pragma once

#include <cmath>

using namespace std;

class MomentoResistente 
{
private:
    double fck;   // Resistência do concreto (MPa)
    double fyk;   // Resistência do aço (MPa)
    double bw;     // Largura da seção (cm)
    double d;     // Altura útil (cm)
    double As;    // Área de aço (cm²)

public:
    MomentoResistente(double fck, double fyk, double bw, double d, double As);
    double calcularMomentoResistente();  // Retorna o momento resistente (kN.m)
};