#pragma once

#include "Point.h"

#include <cmath>
#include <vector>

enum class StressStrainModelType
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
	double strainConcretePlastic;
	double strainConcreteRupture;
	double strengthReductionFactor;
	double stressStrainExponent;
	vector<Point> curveStressStrain;
	StressStrainModelType modelType;

public:
	ConcreteProperties();

	void setParameters(StressStrainModelType model, double collectedFck, double collectedGammaC);
	double computeStress(double strain);
	void setCurveStressStrain();

	double getFck() const;
	double getGammaC() const;
	double getFcd() const;
	double getStrainConcretePlastic() const;
	double getStrainConcreteRupture() const;
	double getStrenghtReductionFactor() const;
	double getStressStrainExponent() const;
	vector<Point> getCurveStressStrain() const;
};

