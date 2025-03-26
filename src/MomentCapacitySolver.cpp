#include "MomentCapacitySolver.h"

MomentCapacitySolver::MomentCapacitySolver()
{
    maxIterations = 0;
    iterations = 0;
    tolerance = 0;
    axialForceSum = 0;
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

    std::cout << "eps1: " << strainDistribution.getTopFiberStrain() << std::endl;
    std::cout << "eps2: " << strainDistribution.getBottomFiberStrain() << std::endl;
    std::cout << "Soma limite regiao 1-2: " << axialForceRegion12Sum << std::endl;

    const auto &vectorReinforcement = reinforcement.getReinforcement();
    double temp = 0;
    for (size_t i = 0; i < vectorReinforcement.size(); i++)
    {
        if (vectorReinforcement[i].getY() <= temp)
            temp = vectorReinforcement[i].getY();
    }

    double effectiveDepth = polygon.getMaxY() - temp;

    double eps1Region23 = -concrete.getStrainConcreteRupture();
    double eps2Region23 = ((steel.getStrainSteelRupture() - eps1Region23) *
                           (polygon.getPolygonHeight() / effectiveDepth)) +
                          eps1Region23;

    double axialForceRegion23Sum = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                              analyticalIntegration, internalForces, eps1Region23, eps2Region23, Nsd);

    std::cout << "eps1: " << strainDistribution.getTopFiberStrain() << std::endl;
    std::cout << "eps2: " << strainDistribution.getBottomFiberStrain() << std::endl;
    std::cout << "Soma limite regiao 2-3: " << axialForceRegion23Sum << std::endl;

    if (axialForceRegion23Sum <= 0)
    {
        std::cout << "Regiao 3" << std::endl;
        // Mrd = iterateInRegion3(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
           //                    analyticalIntegration, internalForces, Nsd);
        Mrd = testRegion3(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
    }
    else if (axialForceRegion12Sum <= 0)
    {
        std::cout << "Regiao 2" << std::endl;
        // Mrd = iterateInRegion2(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
        //                        analyticalIntegration, internalForces, Nsd);
        Mrd = testRegion2(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
    }
    else
    {
        std::cout << "Regiao 1" << std::endl;
        // Mrd = iterateInRegion1(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
        //                        analyticalIntegration, internalForces, Nsd);
        Mrd = testRegion1(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
    }
}

double MomentCapacitySolver::computeAxialForceResultant(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel,
                                                        StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
                                                        InternalForces &internalForces, double strain1, double strain2, double Nsd)
{
    if (strain1 == strain2 && strain1 == -concrete.getStrainConcretePlastic())
    {
        internalForces.setNormalSolicitation(Nsd);
        internalForces.computeMaxCompression(polygon, reinforcement, steel, concrete);
        return internalForces.getMaxNormalCompression() - Nsd;
    }
    else if (strain1 == strain2 && strain1 == steel.getStrainSteelRupture())
    {
        internalForces.setNormalSolicitation(Nsd);
        internalForces.computeMaxTraction(polygon, reinforcement, steel);
        return internalForces.getMaxNormalTraction() - Nsd;
    }
    else
    {
        setStrainDistribution(polygon, concrete, strainDistribution, strain1, strain2);
        setStressRegions(polygon, strainDistribution, stressRegions);
        setInternalForces(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions, analyticalIntegration, internalForces, Nsd);
        return internalForces.getNormalConcrete() + internalForces.getNormalSteel() - Nsd;
    }
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
                                              InternalForces &internalForces, double Nsd)
{
    double epcu = -concrete.getStrainConcreteRupture();
    double epc2 = -concrete.getStrainConcretePlastic();
    double h = polygon.getPolygonHeight();

    double xpRegion1 = h * ((epcu - epc2) / (epcu));
    double ypRegion1 = epc2;

    double ak = (epc2 - epc2) / h;
    double epsAak = ypRegion1 + ak * (-xpRegion1);
    double epsBak = ypRegion1 + ak * (h - xpRegion1);
    double fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsAak, epsBak, Nsd);

    double bk = (-epcu) / h;
    double epsAbk = ypRegion1 + bk * (-xpRegion1);
    double epsBbk = ypRegion1 + bk * (h - xpRegion1);
    double fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsAbk, epsBbk, Nsd);

    double ck = 0;
    double epsAck = 0;
    double epsBck = 0;
    double fC = 0;

    iterations = 0;
    maxIterations = 100;
    tolerance = 0.001;
    axialForceSum = tolerance + 1;

    if (fA * fB < 0)
    {
        while (iterations < maxIterations && abs(axialForceSum) > tolerance)
        {
            iterations++;

            ck = bk - ((fB * (bk - ak)) / (fB - fA));
            epsAck = ypRegion1 + ck * (-xpRegion1);
            epsBck = ypRegion1 + ck * (h - xpRegion1);
            fC = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                            analyticalIntegration, internalForces, epsAck, epsBck, Nsd);

            if (fA * fC > 0)
            {
                ak = ck;
                epsAak = ypRegion1 + ak * (-xpRegion1);
                epsBak = ypRegion1 + ak * (h - xpRegion1);
                fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                       analyticalIntegration, internalForces, epsAak, epsBak, Nsd);
            }
            else
            {
                bk = ck;
                epsAbk = ypRegion1 + bk * (-xpRegion1);
                epsBbk = ypRegion1 + bk * (h - xpRegion1);
                fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                       analyticalIntegration, internalForces, epsAbk, epsBbk, Nsd);
            }

            axialForceSum = fC;
        }

        converged = true;

        if (iterations == maxIterations)
            std::cout << "Equacao de equilibrio nao convergiu dentro do limite de iteracoes";

        epsAck = ypRegion1 + ck * (-xpRegion1);
        epsBck = ypRegion1 + ck * (h - xpRegion1);

        strainResult.setStrain(epsAck, epsBck);
        return computeMomentResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                      analyticalIntegration, internalForces, epsAck, epsBck, Nsd);
    }
    else
    {
        converged = false;
        std::cout << "Error: metodo da falsa posicao requer fak * fbk < 0 para garantir uma raiz no intervalo";
        return 0;
    }
}

double MomentCapacitySolver::iterateInRegion2(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel,
                                              StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
                                              InternalForces &internalForces, double Nsd)
{
    double epsA = -concrete.getStrainConcreteRupture();

    double epsBmin = 0;
    double epsBmax = steel.getStrainSteelRupture();

    double fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsA, epsBmin, Nsd);

    double fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsA, epsBmax, Nsd);

    double ck = 0;
    double fC = 0;

    iterations = 0;
    maxIterations = 100;
    tolerance = 0.001;
    axialForceSum = tolerance + 1;

    if (fA * fB < 0)
    {
        while (iterations < maxIterations && abs(axialForceSum) > tolerance)
        {
            iterations++;

            ck = epsBmax - ((fB * (epsBmax - epsBmin))) / (fB - fA);

            fC = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                            analyticalIntegration, internalForces, epsA, ck, Nsd);

            if (fA * fC > 0)
            {
                epsBmin = ck;
                fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                analyticalIntegration, internalForces, epsA, ck, Nsd);
            }
            else
            {
                epsBmax = ck;
                fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                analyticalIntegration, internalForces, epsA, ck, Nsd);
            }

            axialForceSum = fC;
        }

        converged = true;

        if (iterations == maxIterations)
            std::cout << "Equacao de equilibrio nao convergiu dentro do limite de iteracoes";

        strainResult.setStrain(epsA, ck);
        return computeMomentResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                      analyticalIntegration, internalForces, epsA, ck, Nsd);
    }
    else
    {
        converged = false;
        std::cout << "Error: metodo da falsa posicao requer fak * fbk < 0 para garantir uma raiz no intervalo";
        return 0;
    }
}

double MomentCapacitySolver::iterateInRegion3(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel,
                                              StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration,
                                              InternalForces &internalForces, double Nsd)
{
    const auto &vectorReinforcement = reinforcement.getReinforcement();
    double temp = 0;

    for (size_t i = 0; i < vectorReinforcement.size(); i++)
    {
        if (vectorReinforcement[i].getY() <= temp)
            temp = vectorReinforcement[i].getY();
    }

    double d = polygon.getMaxY() - temp;
    double h = polygon.getPolygonHeight();
    double epcu = -concrete.getStrainConcreteRupture();
    double epsu = steel.getStrainSteelRupture();

    double epsAmin = epcu;
    double epsBmin = ((epsu - epsAmin) * (h / d)) + epsAmin;
    double fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsAmin, epsBmin, Nsd);

    double epsAmax = epsu;
    double epsBmax = ((epsu - epsAmax) * (h / d)) + epsAmax;
    double fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                           analyticalIntegration, internalForces, epsAmax, epsBmax, Nsd);

    double ck = 0;
    double epsAck = 0;
    double epsBck = 0;
    double fC = 0;

    iterations = 0;
    maxIterations = 100;
    tolerance = 0.001;
    axialForceSum = tolerance + 1;

    if (fA * fB < 0)
    {
        while (iterations < maxIterations && abs(axialForceSum) > tolerance)
        {
            iterations++;

            ck = epsAmax - ((fB * (epsAmax - epsAmin)) / (fB - fA));
            epsAck = ck;
            epsBck = ((epsu - epsAck) * (h / d)) + epsAck;
            fC = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                            analyticalIntegration, internalForces, epsAck, epsBck, Nsd);

            if (fA * fC > 0)
            {
                epsAmin = ck;
                epsBmin = ((epsu - ck) * (h / d)) + ck;
                fA = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                analyticalIntegration, internalForces, epsAmin, epsBmin, Nsd);
            }
            else
            {
                epsAmax = ck;
                epsBmax = ((epsu - ck) * (h / d)) + ck;
                fB = computeAxialForceResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                                analyticalIntegration, internalForces, epsAmax, epsBmax, Nsd);
            }

            axialForceSum = fC;
        }

        converged = true;

        if (iterations == maxIterations)
            std::cout << "Equacao de equilibrio nao convergiu dentro do limite de iteracoes";

        strainResult.setStrain(ck, (((epsu - ck) * (h / d)) + ck));
        return computeMomentResultant(polygon, reinforcement, concrete, steel, strainDistribution, stressRegions,
                                      analyticalIntegration, internalForces, ck, (((epsu - ck) * (h / d)) + ck), Nsd);
    }
    else
    {
        converged = false;
        std::cout << "Error: metodo da falsa posicao requer fak * fbk < 0 para garantir uma raiz no intervalo";
        return 0;
    }
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
    stressRegions.generateStressRegions();
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

double MomentCapacitySolver::findRootBrent(std::function<double(double)> func, double a, double b, double tol, int maxIter)
{
    double fa = func(a);
    double fb = func(b);

    if (fa * fb >= 0.0)
    {
        std::cerr << "Erro: Brent requer fa * fb < 0 (raiz não isolada no intervalo)" << std::endl;
        return a;
    }

    double c = a;
    double fc = fa;
    double s = b;
    double fs = fb;
    double d = b - a;
    bool mflag = true;

    for (int iter = 0; iter < maxIter; ++iter)
    {
        if (fa != fc && fb != fc)
        {
            // Interpolação inversa quadrática
            s = a * fb * fc / ((fa - fb) * (fa - fc))
              + b * fa * fc / ((fb - fa) * (fb - fc))
              + c * fa * fb / ((fc - fa) * (fc - fb));
        }
        else
        {
            // Método da secante
            s = b - fb * (b - a) / (fb - fa);
        }

        double cond1 = (s < (3 * a + b) / 4 || s > b);
        double cond2 = (mflag && std::abs(s - b) >= std::abs(b - c) / 2);
        double cond3 = (!mflag && std::abs(s - b) >= std::abs(c - d) / 2);
        double cond4 = (mflag && std::abs(b - c) < tol);
        double cond5 = (!mflag && std::abs(c - d) < tol);

        if (cond1 || cond2 || cond3 || cond4 || cond5) 
        {
            // Bisseção
            s = (a + b) / 2;
            mflag = true;
        } 
        else 
        {
            mflag = false;
        }

        fs = func(s);
        d = c;
        c = b;
        fc = fb;

        if (fa * fs < 0) 
        {
            b = s;
            fb = fs;
        } 
        else 
        {
            a = s;
            fa = fs;
        }

        if (std::abs(fa) < std::abs(fb)) 
        {
            std::swap(a, b);
            std::swap(fa, fb);
        }

        if (std::abs(b - a) < tol || std::abs(fs) < tol) 
        {
            iterations = iter;
            return s;
        }

        iterations = iter;
    }

    std::cerr << "Brent não convergiu em " << maxIter << " iterações." << std::endl;
    return s;
}

double MomentCapacitySolver::testRegion1(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration, InternalForces &internalForces, double Nsd)
{
    double epcu = -concrete.getStrainConcreteRupture();
    double epc2 = -concrete.getStrainConcretePlastic();
    double h = polygon.getPolygonHeight();

    double x_plastic = h * ((epcu - epc2) / epcu);
    double y_plastic = epc2;

    auto func = [&](double k) {
        double epsTop = y_plastic + k * (-x_plastic);
        double epsBottom = y_plastic + k * (h - x_plastic);
        return computeAxialForceResultant(
            polygon, reinforcement, concrete, steel,
            strainDistribution, stressRegions,
            analyticalIntegration, internalForces,
            epsTop, epsBottom, Nsd
        );
    };

    double k_min = (-epcu) / h;
    double k_max = 0;

    if (func(k_min) * func(k_max) >= 0.0) {
        converged = false;
        std::cout << "Erro: Brent - raiz não isolada no intervalo (Região 1)" << std::endl;
        return 0;
    }

    iterations = 0;
    maxIterations = 100;
    tolerance = 1e-5;

    double k_root = findRootBrent(func, k_min, k_max, tolerance, maxIterations);
    double epsTop = y_plastic + k_root * (-x_plastic);
    double epsBottom = y_plastic + k_root * (h - x_plastic);

    strainResult.setStrain(epsTop, epsBottom);
    converged = true;

    return computeMomentResultant(
        polygon, reinforcement, concrete, steel,
        strainDistribution, stressRegions,
        analyticalIntegration, internalForces,
        epsTop, epsBottom, Nsd
    );
}

double MomentCapacitySolver::testRegion2(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration, InternalForces &internalForces, double Nsd)
{
    double epsA = -concrete.getStrainConcreteRupture(); // fixo
    double epsBmin = 0;
    double epsBmax = steel.getStrainSteelRupture();

    // Lambda com captura de contexto
    auto func = [&](double epsB) 
    {
        return computeAxialForceResultant(
            polygon, reinforcement, concrete, steel,
            strainDistribution, stressRegions,
            analyticalIntegration, internalForces,
            epsA, epsB, Nsd
        );
    };

    // Verifica se há mudança de sinal (raiz isolada)
    if (func(epsBmin) * func(epsBmax) >= 0.0) 
    {
        converged = false;
        std::cout << "Erro: função não muda de sinal no intervalo inicial (Region 2)" << std::endl;
        return 0;
    }

    iterations = 0;
    maxIterations = 100;
    tolerance = 1e-4;

    double rootEpsB = findRootBrent(func, epsBmin, epsBmax, tolerance, maxIterations);
    converged = true;

    strainResult.setStrain(epsA, rootEpsB);
    return computeMomentResultant(
        polygon, reinforcement, concrete, steel,
        strainDistribution, stressRegions,
        analyticalIntegration, internalForces,
        epsA, rootEpsB, Nsd
    );
}

double MomentCapacitySolver::testRegion3(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &concrete, SteelProperties &steel, StrainDistribution &strainDistribution, PolygonStressRegions &stressRegions, AnalyticalIntegration &analyticalIntegration, InternalForces &internalForces, double Nsd)
{
    const auto &vectorReinf = reinforcement.getReinforcement();
    double yLowestRebar = 0;
    for (const auto &bar : vectorReinf) 
    {
        if (bar.getY() <= yLowestRebar)
            yLowestRebar = bar.getY();
    }

    double d = polygon.getMaxY() - yLowestRebar;
    double h = polygon.getPolygonHeight();
    double epcu = -concrete.getStrainConcreteRupture();
    double epsu = steel.getStrainSteelRupture();

    auto func = [&](double epsTop) 
    {
        double epsBottom = ((epsu - epsTop) * (h / d)) + epsTop;
        return computeAxialForceResultant(
            polygon, reinforcement, concrete, steel,
            strainDistribution, stressRegions,
            analyticalIntegration, internalForces,
            epsTop, epsBottom, Nsd
        );
    };

    double epsTopMin = epcu;
    double epsTopMax = epsu;

    if (func(epsTopMin) * func(epsTopMax) >= 0.0) 
    {
        converged = false;
        std::cout << "Erro: Brent - raiz não isolada no intervalo (Região 3)" << std::endl;
        return 0;
    }

    iterations = 0;
    maxIterations = 100;
    tolerance = 1e-4;

    double rootEpsTop = findRootBrent(func, epsTopMin, epsTopMax, tolerance, maxIterations);
    double epsBottom = ((epsu - rootEpsTop) * (h / d)) + rootEpsTop;

    strainResult.setStrain(rootEpsTop, epsBottom);
    converged = true;

    return computeMomentResultant(
        polygon, reinforcement, concrete, steel,
        strainDistribution, stressRegions,
        analyticalIntegration, internalForces,
        rootEpsTop, epsBottom, Nsd
    );
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
