#include "model.h"

MomentoResistente::MomentoResistente(double fck, double fyk, double bw, double d, double As) : fck(fck), fyk(fyk), bw(bw), d(d), As(As) {}

double MomentoResistente::calcularMomentoResistente()
{
    double fcd = fck / 1.4;
    double fyd = fyk / 1.15;
    double x = (As * fyd) / (0.85 * fcd * bw);
    double momento = As * fyd * (d - 0.5 * x) / 100; //kN*m
    
    return momento;
}