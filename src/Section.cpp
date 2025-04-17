#include "Section.h"

Section::Section()
{
    originalPolygon = Polygon();
    workingPolygon = Polygon();
    originalReinforcement = Reinforcement();
    workingReinforcement = Reinforcement();
    concrete = ConcreteProperties();
    steel = SteelProperties();
    strainDistribution = StrainDistribution();
    stressRegions = PolygonStressRegions();
    analyticalIntegration = AnalyticalIntegration();
    internalForces = InternalForces();
    momentSolver = MomentSolver();
}

void Section::defineGeometry(const Polygon &polygon, const Reinforcement &reinforcement)
{
    originalPolygon = polygon;
    originalReinforcement = reinforcement;

    workingPolygon = polygon;
    workingPolygon.computeArea();
    workingPolygon.computeCentroid();
    workingPolygon.translateToCentroid();
    workingPolygon.computeHeight();

    workingReinforcement = reinforcement;
    workingReinforcement.translateToCentroidPolygon(workingPolygon.getGeometricCenter());
    workingReinforcement.computeArea();
}

void Section::defineMaterials(const ConcreteProperties &c, const SteelProperties &s)
{
    concrete = c;
    steel = s;
}

void Section::defineIntegrationModel(NormativeIntegrationVersion modelVersion)
{
    analyticalIntegration.setVersion(modelVersion);
}

void Section::resetWorkingState()
{
    workingPolygon = originalPolygon;
    workingPolygon.computeArea();
    workingPolygon.computeCentroid();
    workingPolygon.translateToCentroid();
    workingPolygon.computeHeight();

    workingReinforcement = originalReinforcement;
    workingReinforcement.translateToCentroidPolygon(workingPolygon.getGeometricCenter());
    workingReinforcement.computeArea();
}

void Section::applyAngleandCenterline(double angleDegrees)
{
    workingPolygon.setAngle(angleDegrees);
    workingPolygon.translateToCentroid();
    workingPolygon.rotateAroundCentroid();
    workingPolygon.computeHeight();

    workingReinforcement.translateToCentroidPolygon(workingPolygon.getGeometricCenter());
    workingReinforcement.computeArea();
    workingReinforcement.rotateAroundCentroidPolygon(angleDegrees);
}

void Section::verifyEquilibrium(double Nsd, double eps1, double eps2)
{
    strainDistribution.setStrain(eps1, eps2);
    strainDistribution.computeStrainDistribution(
        concrete.getStrainConcretePlastic(),
        concrete.getStrainConcreteRupture(),
        workingPolygon.getPolygonHeight());

    stressRegions.setOriginalPolygon(workingPolygon);
    stressRegions.setDeformationHeight(
        strainDistribution.getNeutralAxisCoord(),
        strainDistribution.getPlasticStrainCoord(),
        strainDistribution.getRuptureStrainCoord());
    stressRegions.generateStressRegions();

    internalForces.setNormalSolicitation(Nsd);
    internalForces.computeNormalConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeMomentUUConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeMomentVVConcrete(analyticalIntegration, concrete, stressRegions, strainDistribution);
    internalForces.computeNormalSteel(workingPolygon, workingReinforcement, steel, strainDistribution);
    internalForces.computeMomentUUSteel(workingPolygon, workingReinforcement, steel, strainDistribution);
    internalForces.computeMomentVVSteel(workingPolygon, workingReinforcement, steel, strainDistribution);
    internalForces.computeMomentUUSection();
    internalForces.computeMomentVVSection();
    internalForces.computeMomentXXSection(workingPolygon.getAngle());
    internalForces.computeMomentYYSection(workingPolygon.getAngle());
    internalForces.computeMaxCompression(workingPolygon, workingReinforcement, steel, concrete);
    internalForces.computeMaxTraction(workingPolygon, workingReinforcement, steel);
}

void Section::computeEquilibrium(double Nsd, double eps1, double eps2)
{
    verifyEquilibrium(Nsd, +0.003, -0.003); // eps1, eps2

    momentSolver.solveEquilibrium(
        workingPolygon, workingReinforcement, concrete, steel,
        strainDistribution, stressRegions, analyticalIntegration,
        internalForces, Nsd);

    std::cout << "--------------------------------------------\n";
    std::cout << "Angulo: " << workingPolygon.getAngle() << "; "
                  << "eps1: " << momentSolver.getTopFiberStrain() << "; "
                  << "eps2: " << momentSolver.getBottomFiberStrain() << "; "
                  << "Mrdxx: " << momentSolver.getMoment().getX() << "; "
                  << "Mrdyy: " << momentSolver.getMoment().getY() << endl;
    std::cout << "--------------------------------------------\n";
}

void Section::computeEnvelope(double Nsd)
{
    envelopeMoments.clear();

    const std::vector<double> angles = {
        0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57,
        60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111,
        114, 117, 120, 123, 126, 129, 132, 135, 138, 141, 144, 147, 150, 153, 156,
        159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 189, 192, 195, 198, 201,
        204, 207, 210, 213, 216, 219, 222, 225, 228, 231, 234, 237, 240, 243, 246,
        249, 252, 255, 258, 261, 264, 267, 270, 273, 276, 279, 282, 285, 288, 291,
        294, 297, 300, 303, 306, 309, 312, 315, 318, 321, 324, 327, 330, 333, 336,
        339, 342, 345, 348, 351, 354, 357, 360
    };

    cout << "--------------------------------------------\n";

    for (double angle : angles)
    {
        resetWorkingState();
        applyAngleandCenterline(angle);

        computeEquilibrium(Nsd, +0.003, -0.003);

        envelopeMoments.push_back(momentSolver.getMoment());
    }

    cout << "--------------------------------------------\n";
}

// void Section::computeSectionEquilibriumSolver(double Nsd)
// {
//     momentSolver.solveEquilibrium(workingPolygon, workingReinforcement, concrete, steel, strainDistribution, stressRegions,
//     analyticalIntegration, internalForces, Nsd);
//     //envelopeMoments.push_back(momentSolver.getMoment());
//     // std::cout << "Mrdxx: " << momentSolver.getMoment().getX() << " kN*m" << std::endl;
//     // std::cout << "Mrdyy: " << momentSolver.getMoment().getY() << " kN*m" << std::endl;
//     // std::cout << "--------------------------------------------\n";
// }

// void Section::computeSectionMomentEnvelope(double Nsd)
// {
//     envelopeMoments.clear(); // limpar antes de começar

//     vector<double> angles = {0, 20, 40, 60, 80, 100, 120, 140, 160, 183, 200, 220, 240, 260, 280, 300, 320, 340};

//     Polygon originalPolygon = originalPolygon;
//     Reinforcement originalReinforcement = originalReinforcement;

//     for (size_t i = 0; i < angles.size(); i++)
//     {
//         Polygon workingPolygon = originalPolygon;
//         Reinforcement workingReinforcement = originalReinforcement;

//         workingPolygon.setAngle(angles[i]);
//         workingPolygon.translateToCentroid();
//         workingPolygon.rotateAroundCentroid();
//         workingPolygon.computeHeight();

//         workingReinforcement.translateToCentroidPolygon(workingPolygon.getGeometricCenter());
//         workingReinforcement.computeArea();
//         workingReinforcement.rotateAroundCentroidPolygon(angles[i]);

//         this->setPolygon(workingPolygon);
//         this->setReinforcement(workingReinforcement);

//         setStressRegions();

//         computeSectionEquilibriumSolver(Nsd);

//         Point moment = momentSolver.getMoment();

//         envelopeMoments.push_back(moment);

//         std::cout << "Angulo: " << angles[i] << "; "
//         << "eps1: " << momentSolver.getTopFiberStrain() << "; "
//         << "eps2: " << momentSolver.getBottomFiberStrain() << "; "
//         << "Mrdxx: " << moment.getX() << "; "
//         << "Mrdyy: " << moment.getY() << endl;
//         // std::cout << "--------------------------------------------\n";
//     }
// }

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
    cout << "Altura secao: " << workingPolygon.getPolygonHeight() << endl;
    cout << "Altura util: " << workingReinforcement.getEffectiveDepth() << endl;
    cout << "yMaximo: " << workingPolygon.getMaxY() << endl;
    cout << "yMinimo: " << workingPolygon.getMinY() << endl;
    cout << "Area: " << workingPolygon.getPolygonArea() << endl;
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
    cout << "Mrdxx: " << internalForces.getMomentXXSection() << " kN*m" << endl;
    cout << "Mrdyy: " << internalForces.getMomentYYSection() << " kN*m" << endl;
    cout << "--------------------------------------------\n";
    combinations = std::vector<Combination>();
}