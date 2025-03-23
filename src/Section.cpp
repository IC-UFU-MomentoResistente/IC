#include "Section.h"

Section::Section()
{
    polygon = Polygon();
    reinforcement = Reinforcement();
    concrete = ConcreteProperties();
    steel = SteelProperties();
    strainDistribution = StrainDistribution();
    stressRegions = PolygonStressRegions();
    analyticalIntegration = AnalyticalIntegration();
    internalForces = InternalForces();
}

void Section::setPolygon(Polygon collectedPolygon)
{
    polygon = collectedPolygon;

    polygon.computeCentroid();
    polygon.translateToCentroid();
    polygon.computeMaxCoordY();
    polygon.computeMinCoordY();
    polygon.computeHeight();
    polygon.computeArea();
}

void Section::setReinforcement(Reinforcement collectedReinforcement)
{
    reinforcement = collectedReinforcement;

    reinforcement.computeArea();
    reinforcement.translateToCentroidPolygon(polygon.getGeometricCenter());
}

void Section::setConcrete(ConcreteProperties collectedConcrete)
{
    concrete = collectedConcrete;
}

void Section::setSteel(SteelProperties collectedSteel)
{
    steel = collectedSteel;
}

void Section::setStrainDistribution(double strain1, double strain2)
{
    strainDistribution.setStrain(strain1, strain2);
    strainDistribution.computeStrainDistribution(concrete.getStrainConcretePlastic(),
                                                 concrete.getStrainConcreteRupture(), polygon.getPolygonHeight());
}

void Section::setStressRegions()
{
    stressRegions.setOriginalPolygon(polygon);
    stressRegions.setDeformationHeight(strainDistribution.getNeutralAxisCoord(),
                                       strainDistribution.getPlasticStrainCoord(), strainDistribution.getRuptureStrainCoord());
    stressRegions.generateStressRegions();
}

void Section::setIntegrationVersion(NormativeIntegrationVersion modelVersion)
{
    analyticalIntegration.setVersion(modelVersion);
}

void Section::computeInternalForces(double Nsd)
{
    internalForces.setNormalSolicitation(Nsd);
    internalForces.computeNormalConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeMomentConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeNormalSteel(polygon, reinforcement, steel, strainDistribution);
    internalForces.computeMomentSteel(polygon, reinforcement, steel, strainDistribution);
    internalForces.computeMaxCompression(polygon, reinforcement, steel, concrete);
    internalForces.computeMaxTraction(polygon, reinforcement, steel);
}

void Section::printSectionData()
{
    cout << "--------------------------------------------\n";
    cout << "Secao: " << endl;
    
    cout << "\nVertices:" << endl;

    for (size_t i = 0; i < stressRegions.getOriginalPolygon().getPolygonVertices().size(); i++)
    {
        cout << i << ": " 
        << stressRegions.getOriginalPolygon().getPolygonVertices()[i].getX() << " " 
        << stressRegions.getOriginalPolygon().getPolygonVertices()[i].getY() << endl;
    }

    cout << "\nRegiao comprimida:" << endl;

    for (size_t i = 0; i < stressRegions.getCompressedRegion().getPolygonVertices().size(); i++)
    {
        cout << i << ": " 
        << stressRegions.getCompressedRegion().getPolygonVertices()[i].getX() << " " 
        << stressRegions.getCompressedRegion().getPolygonVertices()[i].getY() << endl;
    }

    cout << "\nRegiao parabolica:" << endl;

    for (size_t i = 0; i < stressRegions.getParabolicRegion().getPolygonVertices().size(); i++)
    {
        cout << i << ": " 
        << stressRegions.getParabolicRegion().getPolygonVertices()[i].getX() << " " 
        << stressRegions.getParabolicRegion().getPolygonVertices()[i].getY() << endl;
    }

    cout << "\nRegiao retangular:" << endl;

    for (size_t i = 0; i < stressRegions.getRectangularRegion().getPolygonVertices().size(); i++)
    {
        cout << i << ": " 
        << stressRegions.getRectangularRegion().getPolygonVertices()[i].getX() << " " 
        << stressRegions.getRectangularRegion().getPolygonVertices()[i].getY() << endl;
    }

    cout << "--------------------------------------------\n";
    cout << "Altura secao: " << polygon.getPolygonHeight() << endl;
    cout << "yMaximo: " << polygon.getMaxY() << endl;
    cout << "yMinimo: " << polygon.getMinY() << endl;
    cout << "Area: " << polygon.getPolygonArea() << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Parametros Concreto: \n";
    cout << "fcd: " << concrete.getFcd() << " MPa" << endl;
    cout << "Fator mult: " << concrete.getFactorMultiplierFcd() << endl;
    cout << "Exp: " << concrete.getStressStrainExponent() << endl;
    cout << "Eps Ultimo: " << concrete.getStrainConcreteRupture() << endl;
    cout << "Eps Limite " << concrete.getStrainConcretePlastic() << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Alturas Deformacao (Concreto): \n";
    cout << "xAlpha: " << strainDistribution.getNeutralAxisCoord() << endl;
    cout << "xEpsLimite: " << strainDistribution.getPlasticStrainCoord() << endl;
    cout << "xEpsUltimo: " << strainDistribution.getRuptureStrainCoord() << endl;
    cout << "hLN: " << stressRegions.getNeutralAxisHeight() << endl;
    cout << "hEpsLimite: " << stressRegions.getPlasticHeight() << endl;
    cout << "hEpsUltimo: " << stressRegions.getRuptureHeight() << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Forca e Momento (Concreto): \n";
    cout << "Normal: " << internalForces.getNormalConcrete() << " kN" << endl;
    cout << "Momento: " << internalForces.getMomentConcrete() << " kN*m" << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Parametros Aco Passivo: \n";
    cout << "fyd: " << steel.getFyd() << " MPa" << endl;
    cout << "elasY: " << steel.getE() << " GPa" << endl;
    cout << "epsYd: " << steel.getStrainSteelYield() << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Forca e Momento (Aco Passivo): \n";
    cout << "Normal: " << internalForces.getNormalSteel() << " kN" << endl;
    cout << "Momento: " << internalForces.getMomentSteel() << " kN*m" << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Normal Minima e Maxima: \n";
    cout << "Maxima compressao: " << internalForces.getMaxNormalCompression() << " kN" << endl;
    cout << "Maxima tracao: " << internalForces.getMaxNormalTraction() << " kN" << endl;
    cout << "--------------------------------------------\n";

    cout << "--------------------------------------------\n";
    cout << "Equilibrio: \n";
    cout << "Normal: " << internalForces.getNormalSection() << " kN" << endl;
    cout << "Momento: " << internalForces.getMomentSection() << " kN*m" << endl;
    cout << "--------------------------------------------\n";
}