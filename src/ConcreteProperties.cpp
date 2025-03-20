#include "ConcreteProperties.h"

#include "Point.h"

#include <cmath>;
#include <vector>

ConcreteProperties::ConcreteProperties()
{
	fck = 0;
	gammaC = 0;
	fcd = 0;
	strainConcretePlastic = 0;
	strainConcreteRupture = 0;
	strengthReductionFactor = 0;
	stressStrainExponent = 0;
	curveStressStrain = {};
}

void ConcreteProperties::setParameters(double collectedFck, double collectedGammaC)
{
	if (collectedFck <= 50)
	{
		fck = collectedFck;
		gammaC = collectedGammaC;
		fcd = collectedFck / collectedGammaC;
		strainConcretePlastic = 2;
		strainConcreteRupture = 3.5;
	}
	else
	{
		fck = collectedFck;
		gammaC = collectedGammaC;
		fcd = collectedFck / collectedGammaC;
		strainConcretePlastic = 2 + 0.085 * pow((collectedFck - 50), 0.53);
		strainConcreteRupture = 2.6 + 35 * pow(((90 - collectedFck) / 100), 4);

		if (strainConcretePlastic > strainConcreteRupture)
			strainConcretePlastic = strainConcreteRupture;
	}
}

double ConcreteProperties::computeStress(StressStrainModelType model, double strain)
{
	double absStrain = -strain;

	if (absStrain <= 0)
		return 0;

	switch (model)
	{
	case StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2014:

		if (fck <= 50)
			stressStrainExponent = 2;
		else if (fck > 50)
			stressStrainExponent = 1.4 + 23.4 * pow(((90 - fck) / 100), 4);

		if (absStrain <= strainConcretePlastic)
			return 0.85 * fcd * (1 - (pow((1 - (absStrain / strainConcretePlastic)), stressStrainExponent)));
		else if (absStrain <= strainConcreteRupture)
			return 0.85 * fcd;

		return 0;
		break;

	case StressStrainModelType::PARABOLA_RECTANGLE_NBR6118_2023:

		if (fck <= 40)
			strengthReductionFactor = 1;
		else if (fck > 40)
			strengthReductionFactor = pow((40 / fck), (1 / 3));

		if (fck <= 50)
			stressStrainExponent = 2;
		else if (fck > 50)
			stressStrainExponent = 1.4 + 23.4 * pow(((90 - fck) / 100), 4);

		if (absStrain <= strainConcretePlastic)
			return 0.85 * strengthReductionFactor * fcd * (1 - (pow((1 - (absStrain / strainConcretePlastic)), stressStrainExponent)));
		else if (absStrain <= strainConcreteRupture)
			return 0.85 * strengthReductionFactor * fcd;

		return 0;
		break;

	default:
		return 0;
	}
}

void ConcreteProperties::setCurveStressStrain(StressStrainModelType model)
{
	double step = (strainConcreteRupture - 0) / 60;

	for (double strain = 0; strain >= -3.5; strain -= step)
	{
		double stress = computeStress(model, strain);

		curveStressStrain.push_back(Point(-strain, stress));
	}
}

double ConcreteProperties::getFck() const
{
	return fck;
}

double ConcreteProperties::getGammaC() const
{
	return gammaC;
}

double ConcreteProperties::getFcd() const
{
	return fcd;
}

double ConcreteProperties::getStrainConcretePlastic() const
{
	return strainConcretePlastic;
}

double ConcreteProperties::getStrainConcreteRupture() const
{
	return strainConcreteRupture;
}

double ConcreteProperties::getStrenghtReductionFactor() const
{
	return strengthReductionFactor;
}

double ConcreteProperties::getStressStrainExponent() const
{
	return stressStrainExponent;
}

vector<Point> ConcreteProperties::getCurveStressStrain() const
{
	return curveStressStrain;
}