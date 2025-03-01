#include "controller.h"

Controller::Controller() : fck(30), fyk(500), bw(30), d(50), As(5), momentoResistente(0) {}

void Controller::setDados(double fck, double fyk, double bw, double d, double As)
{
    this->fck = fck;
    this->fyk = fyk;
    this->bw = bw;
    this->d = d;
    this->As = As;
}

void Controller::calcularMonento()
{
    MomentoResistente mr(fck, fyk, bw, d, As); // instancia da classe model
    momentoResistente = mr.calcularMomentoResistente(); // instancia da variavel momento resistente da controller
}

double Controller::getMomento() { return momentoResistente; }