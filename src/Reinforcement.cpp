#include "Reinforcement.h"

Reinforcement::Reinforcement()
{
	reinforcement = {};
	diameters = {};
	areas = {};
}

void Reinforcement::setReinforcement(vector<Point> collectedReinforcement, vector<double> collectedDiameters)
{
	for (size_t i = 0; i < collectedReinforcement.size(); i++)
	{
		double coordX = collectedReinforcement[i].getX();
		double coordY = collectedReinforcement[i].getY();

		reinforcement.push_back(Point(coordX, coordY));

		double diameter = collectedDiameters[i]; // mm

		diameters.push_back(diameter);
	}
}

void Reinforcement::addReinforcement(double coordX, double coordY, double diameter)
{
	reinforcement.push_back(Point(coordX, coordY));
	diameters.push_back(diameter);
}

void Reinforcement::removeLastReinforcement()
{
	reinforcement.pop_back();
	diameters.pop_back();
	areas.pop_back();
}

void Reinforcement::clearReinforcement()
{
	reinforcement.clear();
	diameters.clear();
	areas.clear();
}

void Reinforcement::computeArea()
{
	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double tempDiameter = diameters[i] / 10; // cm
		double tempArea = pow(tempDiameter, 2) * 3.141592653589793 / 4;
		areas.push_back(tempArea);
	}
}

void Reinforcement::translateToCentroidPolygon(Point centroid)
{
	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double coordX = reinforcement[i].getX() - centroid.getX();
		double coordY = reinforcement[i].getY() - centroid.getY();

		reinforcement[i].setX(coordX);
		reinforcement[i].setY(coordY);
	}
}

void Reinforcement::rotateAroundCentroidPolygon(double angle)
{
	double rad = angle * 3.14159265358979323846 / 180;
	double cosAngle = cos(rad);
	double sinAngle = sin(rad);

	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double rotateX = (reinforcement[i].getX() * cosAngle) - (reinforcement[i].getY() * sinAngle);
		double rotateY = (reinforcement[i].getX() * sinAngle) + (reinforcement[i].getY() * cosAngle);
		
		reinforcement[i].setX(rotateX);
		reinforcement[i].setY(rotateY);
	}
}

vector<Point> Reinforcement::getReinforcement() const
{
	return reinforcement;
}

vector<double> Reinforcement::getDiameters() const
{
	return diameters;
}

vector<double> Reinforcement::getAreas() const
{
	return areas;
}