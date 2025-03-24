#pragma once

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "SteelProperties.h"
#include "StrainDistribution.h"
#include "PolygonStressRegions.h"
#include "AnalyticalIntegration.h"
#include "InternalForces.h"

using std::vector;
using std::cout;
using std::endl;

class Section
{
public:
    Polygon polygon;
    Reinforcement reinforcement;
    ConcreteProperties concrete;
    SteelProperties steel;
    StrainDistribution strainDistribution;
    PolygonStressRegions stressRegions;
    AnalyticalIntegration analyticalIntegration;
    InternalForces internalForces;

    Section();

    void setPolygon(Polygon collectedPolygon);
    void setReinforcement(Reinforcement collectedReinforcement);
    void setConcrete(ConcreteProperties collectedConcrete);
    void setSteel(SteelProperties collectedSteel);
    void setStrainDistribution(double strain1, double strain2);
    void setStressRegions();
    void setIntegrationVersion(NormativeIntegrationVersion modelVersion);
    void computeInternalForces(double Nsd);
    void setSectionProperties(Polygon collectedPolygon, Reinforcement collectedReinforcement, ConcreteProperties collectedConcrete,
    SteelProperties collectedSteel, NormativeIntegrationVersion modelVersion);
    void printSectionData();
};

