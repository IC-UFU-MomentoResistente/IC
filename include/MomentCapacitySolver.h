#pragma once

#include "Section.h"

class MomentCapacitySolver
{
private:
    int maxIterations;
    int iterations;
    double tolerance;
    bool converged;

    double Mrd;
    StrainDistribution strainResult;

public:
    MomentCapacitySolver();

    void solveEquilibrium(Section &section, double Nsd);
    double computeAxialForceResultant(Section &section, double eps1, double eps2, double Nsd);
    double computeMomentResultant(Section &section, double eps1, double eps2, double Nsd);
    double iterateInRegion1(Section &section, double Nsd);
    double iterateInRegion2(Section &section, double Nsd);
    double iterateInRegion3(Section &section, double Nsd);

    int getIterations() const;
    bool isConverged() const;
    double getMomentCapacity() const;
    double getTopFiberStrain() const;
    double getBottomFiberStrain() const;
};