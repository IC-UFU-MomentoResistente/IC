#pragma once

#include "model.h"

class Controller
{
private:
    double fck, fyk, bw, d, As; //dados da secao
    double momentoResistente;

public:
    Controller();

    void setDados(double fck, double fyk, double bw, double d, double As);
    void calcularMonento();
    double getMomento();
};
