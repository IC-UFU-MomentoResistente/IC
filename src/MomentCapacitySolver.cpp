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

void MomentCapacitySolver::solveEquilibrium(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, 
SteelProperties &steel, StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, 
AnalyticalIntegration &analyticalIntegration, InternalForces &internalForces, double Nsd)
{
    double eps1Region12 = -concrete.getStrainConcreteRupture();
    double eps2Region12 = 0;
    double axialForceRegion12Sum = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
    analyticalIntegration, internalForces, eps1Region12, eps2Region12, Nsd);

    const auto& vectorReinforcement = reinforcement.getReinforcement();
    double temp = 0;
    for (size_t i = 0; i < vectorReinforcement.size(); i++)
    {
        if (vectorReinforcement[i].getY() <= temp)
            temp = vectorReinforcement[i].getY();
    }

    double effectiveDepth = polygon.getMaxY() - temp;

    double eps1Region23 = -concrete.getStrainConcreteRupture();
    double eps2Region23 = ((steel.getStrainSteelRupture() - eps1Region23) * 
    (polygon.getPolygonHeight()/effectiveDepth)) + eps1Region23;

    double axialForceRegion23Sum = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, 
    analyticalIntegration, internalForces, eps1Region23, eps2Region23, Nsd);

    if (axialForceRegion23Sum <= 0)
        Mrd = iterateInRegion3(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, 
    analyticalIntegration, internalForces, eps1Region23, eps2Region23, Nsd);
    else if (axialForceRegion12Sum <= 0)
        Mrd = iterateInRegion2(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, 
    analyticalIntegration, internalForces, eps1Region23, eps2Region23, Nsd);
    else 
        Mrd = iterateInRegion1(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, 
    analyticalIntegration, internalForces, eps1Region23, eps2Region23, Nsd);
}

double MomentCapacitySolver::computeAxialForceResultant(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    setStrainDistribution(polygon, concrete, strainDistribution, strain1, strain2);
    setStressRegions(polygon, strainDistribution, stressRegions);
    setInternalForces(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
    return internalForces.getNormalSection() - Nsd;
}

double MomentCapacitySolver::computeMomentResultant(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    setStrainDistribution(polygon, concrete, strainDistribution, strain1, strain2);
    setStressRegions(polygon, strainDistribution, stressRegions);
    setInternalForces(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
    return internalForces.getMomentSection();
}

double MomentCapacitySolver::iterateInRegion1(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    return 0.0;
}

double MomentCapacitySolver::iterateInRegion2(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    return 0.0;
}

double MomentCapacitySolver::iterateInRegion3(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    return 0.0;
}

void MomentCapacitySolver::setStrainDistribution(Polygon &polygon, ConcreteProperties &concrete, StrainDistribution &strainDistribution, 
double strain1, double strain2)
{
    strainDistribution.setStrain(strain1, strain2);
    strainDistribution.computeStrainDistribution(concrete.getStrainConcretePlastic(), 
    concrete.getStrainConcreteRupture(), polygon.getPolygonHeight());
}

void MomentCapacitySolver::setStressRegions(Polygon &polygon, StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions)
{
    stressRegions.setOriginalPolygon(polygon);
    stressRegions.setDeformationHeight(strainDistribution.getNeutralAxisCoord(), 
    strainDistribution.getPlasticStrainCoord(), strainDistribution.getRuptureStrainCoord());
}

void MomentCapacitySolver::setInternalForces(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, 
StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration, 
InternalForces &internalForces, double Nsd)
{
    internalForces.setNormalSolicitation(Nsd);
    internalForces.computeNormalConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeMomentConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeNormalSteel(polygon, reinforcement, steel, strainDistribution);
    internalForces.computeMomentSteel(polygon, reinforcement, steel, strainDistribution);
    internalForces.computeMaxCompression(polygon, reinforcement, steel, concrete);
    internalForces.computeMaxTraction(polygon, reinforcement, steel);
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
