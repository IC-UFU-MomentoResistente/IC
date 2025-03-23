#include "InternalForces.h"

InternalForces::InternalForces()
{
    normalConcrete = 0;
    momentConcrete = 0;
    normalSteel = 0;
    momentSteel = 0;
    maxNormalCompression = 0;
    maxNormalTraction = 0;
    normalSolicitation = 0;
}

void InternalForces::setNormalSolicitation(double Nsd)
{
    normalSolicitation = Nsd;
}

void InternalForces::computeNormalConcrete(AnalyticalIntegration &analyticalIntegration, ConcreteProperties &concrete,
PolygonStressRegions &stressRegions, StrainDistribution &strainDistribution)
{
    double coordLN = strainDistribution.getNeutralAxisCoord(); // mm/m
    double coordEpc2 = strainDistribution.getPlasticStrainCoord(); // mm/m
    double coordEcpu = strainDistribution.getRuptureStrainCoord(); // mm/m
    double fcd = concrete.getFcd(); // MPa
    double multFcd = concrete.getFactorMultiplierFcd();
    double nConc = concrete.getStressStrainExponent();

    double NCTP = 0;
    double NCTR = 0;

    const auto& parabolicRegion = stressRegions.getParabolicRegion().getPolygonVertices();
    const auto& rectangularRegion = stressRegions.getRectangularRegion().getPolygonVertices();

    for (size_t i = 0; i < parabolicRegion.size(); i++)
    {
        int j = (i + 1) % parabolicRegion.size();

        double x1 = parabolicRegion[i].getX() / 100; // m
        double y1 = parabolicRegion[i].getY() / 100; // m
        double x2 = parabolicRegion[j].getX() / 100; // m
        double y2 = parabolicRegion[j].getY() / 100; // m 

        if (y1 - y2 != 0)
        {
            double coef1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
            double coef2 = (x1 - x2) / (y1 - y2); // m

            double nc1 = analyticalIntegration.computeNormalConcreteParabolic(coordLN, coordEpc2, nConc, fcd, multFcd, coef1, coef2, y1);
            double nc2 = analyticalIntegration.computeNormalConcreteParabolic(coordLN, coordEpc2, nConc, fcd, multFcd, coef1, coef2, y2);

            NCTP = NCTP + nc2 - nc1;
        }
    }

    for (size_t i = 0; i < rectangularRegion.size(); i++)
    {
        int j = (i + 1) % rectangularRegion.size();

        double x1 = rectangularRegion[i].getX() / 100; // m
        double y1 = rectangularRegion[i].getY() / 100; // m
        double x2 = rectangularRegion[j].getX() / 100; // m
        double y2 = rectangularRegion[j].getY() / 100; // m

        if (y1 - y2 != 0)
        {
            double coef1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
            double coef2 = (x1 - x2) / (y1 - y2); // m

            double nc1 = analyticalIntegration.computeNormalConcreteRectangular(multFcd, fcd, coef1, coef2, y1);
            double nc2 = analyticalIntegration.computeNormalConcreteRectangular(multFcd, fcd, coef1, coef2, y2);

            NCTR = NCTR + nc2 - nc1;
        }
    }

    normalConcrete = -(NCTP + NCTR);
}

void InternalForces::computeMomentConcrete(AnalyticalIntegration &analyticalIntegration, ConcreteProperties &concrete,
PolygonStressRegions &stressRegions, StrainDistribution &strainDistribution)
{
    double coordLN = strainDistribution.getNeutralAxisCoord(); // mm/m
    double coordEpc2 = strainDistribution.getPlasticStrainCoord(); // mm/m
    double coordEcpu = strainDistribution.getRuptureStrainCoord(); // mm/m
    double fcd = concrete.getFcd(); // MPa
    double multFcd = concrete.getFactorMultiplierFcd();
    double nConc = concrete.getStressStrainExponent();

    double MCTP = 0;
    double MCTR = 0;

    const auto& parabolicRegion = stressRegions.getParabolicRegion().getPolygonVertices();
    const auto& rectangularRegion = stressRegions.getRectangularRegion().getPolygonVertices();

    for (size_t i = 0; i < parabolicRegion.size(); i++)
    {
        int j = (i + 1) % parabolicRegion.size();

        double x1 = parabolicRegion[i].getX() / 100; // m
        double y1 = parabolicRegion[i].getY() / 100; // m
        double x2 = parabolicRegion[j].getX() / 100; // m
        double y2 = parabolicRegion[j].getY() / 100; // m

        if (y1 - y2 != 0)
        {
            double coef1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
            double coef2 = (x1 - x2) / (y1 - y2); // m

            double mc1 = analyticalIntegration.computeMomentConcreteParabolic(coordLN, coordEpc2, nConc, fcd, multFcd, coef1, coef2, y1);
            double mc2 = analyticalIntegration.computeMomentConcreteParabolic(coordLN, coordEpc2, nConc, fcd, multFcd, coef1, coef2, y2);

            MCTP = MCTP + mc2 - mc1;
        }
    }

    for (size_t i = 0; i < rectangularRegion.size(); i++)
    {
        int j = (i + 1) % rectangularRegion.size();

        double x1 = rectangularRegion[i].getX() / 100; // m
        double y1 = rectangularRegion[i].getY() / 100; // m
        double x2 = rectangularRegion[j].getX() / 100; // m
        double y2 = rectangularRegion[j].getY() / 100; // m

        if (y1 - y2 != 0)
        {
            double coef1 = (y1 * x2 - y2 * x1) / (y1 - y2); // m
            double coef2 = (x1 - x2) / (y1 - y2); // m

            double mc1 = analyticalIntegration.computeMomentConcreteRectangular(multFcd, fcd, coef1, coef2, y1);
            double mc2 = analyticalIntegration.computeMomentConcreteRectangular(multFcd, fcd, coef1, coef2, y2);

            MCTR = MCTR + mc2 - mc1;
        }
    }

    momentConcrete = MCTP + MCTR;
}

void InternalForces::computeNormalSteel(Polygon &polygon, Reinforcement &reinforcement, 
SteelProperties &steel, StrainDistribution &strainDistribution)
{
    const auto& reinforcementVertices = reinforcement.getReinforcement();
    const auto& areas = reinforcement.getAreas(); // cm²
    double height = polygon.getPolygonHeight();
    double maxY = polygon.getMaxY();
    double eps1 = strainDistribution.getTopFiberStrain();
    double eps2 = strainDistribution.getBottomFiberStrain();

    double NY = 0;
    double strainReinforcement = 0;
    double k = (eps2 - eps1) / height;

    for (size_t i = 0; i < reinforcementVertices.size(); i++)
    {
        strainReinforcement = (k * (maxY - reinforcementVertices[i].getY())) + eps1; // MPa

        if (strainReinforcement > steel.getStrainSteelRupture())
            strainReinforcement = steel.getStrainSteelRupture();
        else if (strainReinforcement < -steel.getStrainSteelRupture())
            strainReinforcement = -steel.getStrainSteelRupture();
        
        double stressReinforcement = steel.computeStress(strainReinforcement);
        NY = NY + stressReinforcement * areas[i] / 10; // kN'
    }

    normalSteel = NY;
}

void InternalForces::computeMomentSteel(Polygon &polygon, Reinforcement &reinforcement, 
SteelProperties &steel, StrainDistribution &strainDistribution)
{
    const auto& reinforcementVertices = reinforcement.getReinforcement();
    const auto& areas = reinforcement.getAreas(); // cm²
    double height = polygon.getPolygonHeight();
    double maxY = polygon.getMaxY();
    double eps1 = strainDistribution.getTopFiberStrain();
    double eps2 = strainDistribution.getBottomFiberStrain();

    double MY = 0;
    double strainReinforcement = 0;
    double k = (eps2 - eps1) / height;

    for (size_t i = 0; i < reinforcementVertices.size(); i++)
    {
        strainReinforcement = (k * (maxY - reinforcementVertices[i].getY())) + eps1; // MPa

        if (strainReinforcement > steel.getStrainSteelRupture())
            strainReinforcement = steel.getStrainSteelRupture();
        else if (strainReinforcement < -steel.getStrainSteelRupture())
            strainReinforcement = -steel.getStrainSteelRupture();
        
        double stressReinforcement = steel.computeStress(strainReinforcement);
        double ny = stressReinforcement * areas[i] / 10; // kN
        
        MY = MY + (-ny * (reinforcementVertices[i].getY()/100)); // kN.m
    }

    momentSteel = MY;
}

void InternalForces::computeMaxCompression(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel, ConcreteProperties &concrete)
{
    const auto& reinforcementVertices = reinforcement.getReinforcement();
    const auto& areas = reinforcement.getAreas();
    double sectionArea = polygon.getPolygonArea() / 10000; // m^2
    double fcd = concrete.getFcd();
    double multFcd = concrete.getFactorMultiplierFcd();

    double Nap = 0;

    for (size_t i = 0; i < reinforcementVertices.size(); i++)
    {
        double stressReinforcement = steel.computeStress(-concrete.getStrainConcretePlastic());

        Nap += stressReinforcement * areas[i] / 10; // kN
    }

    maxNormalCompression = -(sectionArea * multFcd * fcd) * 1000 + Nap;
}

void InternalForces::computeMaxTraction(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel)
{
    const auto& reinforcementVertices = reinforcement.getReinforcement();
    const auto& areas = reinforcement.getAreas();

    double Nap = 0;

    for (size_t i = 0; i < reinforcementVertices.size(); i++)
    {
        double stressReinforcement = steel.computeStress(steel.getStrainSteelRupture());

        Nap += stressReinforcement * areas[i] / 10; // kN
    }

    maxNormalTraction = normalSteel;
}

double InternalForces::getNormalSection() const
{
    return normalConcrete + normalSteel - normalSolicitation;
}

double InternalForces::getMomentSection() const
{
    return momentConcrete + momentSteel - 0;
}

double InternalForces::getNormalConcrete() const
{
    return normalConcrete;
}

double InternalForces::getMomentConcrete() const
{
    return momentConcrete;
}

double InternalForces::getNormalSteel() const
{
    return normalSteel;
}

double InternalForces::getMomentSteel() const
{
    return momentSteel;
}

double InternalForces::getMaxNormalCompression() const
{
    return maxNormalCompression;
}

double InternalForces::getMaxNormalTraction() const
{
    return maxNormalTraction;
}