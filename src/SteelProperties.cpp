#include "SteelProperties.h"

SteelProperties::SteelProperties() : 
    fyk(500.0), 
    gammaS(1.15), 
    E(210.0), 
    fyd(0.0), 
    strainSteelYield(0.0), 
    strainSteelRupture(0.0)
    
    {}

void SteelProperties::setParameters(StressStrainSteelModelType model, double collectedFyk, double collectedGammaS, double collectedE)
{
    modelType = model;

    fyk = collectedFyk; //MPa
    gammaS = collectedGammaS;
    E = collectedE; //GPa
    fyd = collectedFyk / collectedGammaS; //MPa
    strainSteelYield = fyd / E; //por mil
    strainSteelRupture = 10; //por mil
}

double SteelProperties::computeStress(double strain)
{
    if (-strainSteelRupture <= strain && strain <= -strainSteelYield)
        return -fyd;
    else if (-strainSteelYield < strain && strain < strainSteelYield)
        return E * strain;
    else if (strainSteelYield <= strain && strain <= strainSteelRupture)
        return fyd;
    else
        return 0;
}

void SteelProperties::setCurveStressStrain()
{
    curveStressStrain.clear();

    double step = (strainSteelRupture - (-strainSteelRupture)) / 60;

    for (double strain = -strainSteelRupture; strain <= strainSteelRupture; strain += step)
    {
        double stress = computeStress(strain);

        curveStressStrain.push_back(Point(strain, stress));
    }
}

double SteelProperties::getFyk() const
{
    return fyk;
}

double SteelProperties::getGammaS() const
{
    return gammaS;
}

double SteelProperties::getE() const
{
    return E;
}

double SteelProperties::getFyd() const
{
    return fyd;
}

double SteelProperties::getStrainSteelYield() const
{
    return strainSteelYield;
}

double SteelProperties::getStrainSteelRupture() const
{
    return strainSteelRupture;
}

vector<Point> SteelProperties::getCurveStressStrain() const
{
    return curveStressStrain;
}