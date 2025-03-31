#pragma once

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "SteelProperties.h"
#include "StrainDistribution.h"
#include "PolygonStressRegions.h"
#include "AnalyticalIntegration.h"

class InternalForces
{
private:
    double normalConcrete;
    double momentXXConcrete;
    double momentYYConcrete;
    double normalSteel;
    double momentXXSteel;
    double momentYYSteel;
    double maxNormalCompression;
    double maxNormalTraction;
    double normalSolicitation;

public:
    InternalForces();

    void setNormalSolicitation(double Nsd);
    void computeNormalConcrete(AnalyticalIntegration &analyticalIntegration, ConcreteProperties &concrete,
                               PolygonStressRegions &stressRegions, StrainDistribution &strainDistribution);
    void computeMomentXXConcrete(AnalyticalIntegration &analyticalIntegration, ConcreteProperties &concrete,
                               PolygonStressRegions &stressRegions, StrainDistribution &strainDistribution);
    void computeNormalSteel(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel, 
                            StrainDistribution &strainDistribution);
    void computeMomentSteel(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel, 
                            StrainDistribution &strainDistribution);  
    void computeMaxCompression(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel, 
                               ConcreteProperties &concrete);
    void computeMaxTraction(Polygon &polygon, Reinforcement &reinforcement, SteelProperties &steel);

    double getNormalSection() const;
    double getMomentSection() const;

    double getNormalConcrete() const;
    double getMomentConcrete() const;
    double getNormalSteel() const;
    double getMomentSteel() const;
    double getMaxNormalCompression() const;
    double getMaxNormalTraction() const;
    double getNormalSolicitation() const;
};