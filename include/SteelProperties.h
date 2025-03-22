#pragma once

#include <cmath>
#include <vector>

#include "Point.h"

enum class StressStrainSteelModelType
{
    PASSIVE_REINFORCEMENT,
    ACTIVE_REINFORCEMENT
};

class SteelProperties
{
private:
    double fyk;
    double gammaS;
    double E;
    double fyd;
    double strainSteelYield;
    double strainSteelRupture;
    vector<Point> curveStressStrain;
    StressStrainSteelModelType modelType;

public:
    SteelProperties();

    void setParameters(StressStrainSteelModelType model, double collectedFyk, double collectedGammaS, double collectedE);
    double computeStress(double strain);
    void setCurveStressStrain();
    
    double getFyk() const;
    double getGammaS() const;
    double getE() const;
    double getFyd() const;
    double getStrainSteelYield() const;
    double getStrainSteelRupture() const;
    vector<Point> getCurveStressStrain() const;
};