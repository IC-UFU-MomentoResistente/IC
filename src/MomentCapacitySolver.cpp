#include "MomentCapacitySolver.h"

MomentCapacitySolver::MomentCapacitySolver()
{
    maxIterations = 100;
    iterations = 0;
    tolerance = 1e-3;
    converged = false;
    Mrd = 0;
    strainResult = StrainDistribution();
}

void MomentCapacitySolver::solveEquilibrium(Section &section, double Nsd)
{
    double eps1Region12 = -section.concrete.getStrainConcreteRupture();
    double eps2Region12 = 0;
    double axialForceRegion12Sum = computeAxialForceResultant(section, eps1Region12, eps2Region12, Nsd);

    const auto& vectorReinforcement = section.reinforcement.getReinforcement();
    double temp = 0;
    for (size_t i = 0; i < vectorReinforcement.size(); i++)
    {
        if (vectorReinforcement[i].getY() <= temp)
            temp = vectorReinforcement[i].getY();
    }

    double effectiveDepth = section.polygon.getMaxY() - temp;

    double eps1Region23 = -section.concrete.getStrainConcreteRupture();
    double eps2Region23 = ((section.steel.getStrainSteelRupture() - eps1Region23) * 
    (section.polygon.getPolygonHeight()/effectiveDepth)) + eps1Region23;

    double axialForceRegion23Sum = computeAxialForceResultant(section, eps1Region23, eps2Region23, Nsd);

    if (axialForceRegion23Sum <= 0)
        Mrd = iterateInRegion3(section, Nsd);
    else if (axialForceRegion12Sum <= 0)
        Mrd = iterateInRegion2(section, Nsd);
    else 
        Mrd = iterateInRegion1(section, Nsd);
}

double MomentCapacitySolver::computeAxialForceResultant(Section &section, double eps1, double eps2, double Nsd)
{
    section.setStrainDistribution(eps1, eps2);
    section.setStressRegions();
    section.computeInternalForces(Nsd);
    return section.internalForces.getNormalSection() - Nsd;
}

double MomentCapacitySolver::computeMomentResultant(Section &section, double eps1, double eps2, double Nsd)
{
    section.setStrainDistribution(eps1, eps2);
    section.setStressRegions();
    section.computeInternalForces(Nsd);
    return section.internalForces.getMomentSection();
}

double MomentCapacitySolver::iterateInRegion1(Section &section, double Nsd)
{
    return 0.0;
}

double MomentCapacitySolver::iterateInRegion2(Section &section, double Nsd)
{
    return 0.0;
}

double MomentCapacitySolver::iterateInRegion3(Section &section, double Nsd)
{
    return 0.0;
}

int MomentCapacitySolver::getIterations() const
{
    return iterations;
}

bool MomentCapacitySolver::isConverged() const
{
    return converged;
}

double MomentCapacitySolver::getMomentCapacity() const
{
    return Mrd;
}

double MomentCapacitySolver::getTopFiberStrain() const
{
    return strainResult.getTopFiberStrain();
}

double MomentCapacitySolver::getBottomFiberStrain() const
{
    return strainResult.getBottomFiberStrain();
}
