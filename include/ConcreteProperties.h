#pragma once

#include "Point.h"

#include <cmath>
#include <vector>

enum class StressStrainConcreteModelType
{
	PARABOLA_RECTANGLE_NBR6118_2014,
	PARABOLA_RECTANGLE_NBR6118_2023
};

class ConcreteProperties
{
private:
	double fck;
	double gammaC;
	double fcd;
	double factorMultiplierFcd;
	double strainConcretePlastic;
	double strainConcreteRupture;
	double strengthReductionFactor;
	double stressStrainExponent;
	vector<Point> curveStressStrain;
	StressStrainConcreteModelType modelType;

public:
	ConcreteProperties();

	void setParameters(StressStrainConcreteModelType model, double collectedFck, double collectedGammaC);
	double computeStress(double strain);
	void setCurveStressStrain();

	double getFck() const;
	double getGammaC() const;
	double getFcd() const;
	double getFactorMultiplierFcd() const;
	double getStrainConcretePlastic() const;
	double getStrainConcreteRupture() const;
	double getStrenghtReductionFactor() const;
	double getStressStrainExponent() const;
	vector<Point> getCurveStressStrain() const;
};

