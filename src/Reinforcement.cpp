#include "Reinforcement.h"
#include "Point.h"

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>

using std::vector;

Reinforcement::Reinforcement()
{
	vector<Point> reinforcement = {};
	vector<double> diameters = {};
	vector<double> areas = {};
}

void Reinforcement::setBars(vector<Point> collectedBars, vector<double> collectedDiameters)
{
	for (size_t i = 0; i < collectedBars.size(); i++)
	{
		double coordX = collectedBars[i].getX();
		double coordY = collectedBars[i].getY();

		reinforcement.push_back(Point(coordX, coordY));

		double diameter = collectedDiameters[i];

		diameters.push_back(diameter);
	}
}

void Reinforcement::addReinforcement(double coordX, double coordY, double diameter)
{
	reinforcement.push_back(Point(coordX, coordY));
	diameters.push_back(diameter);
}

void Reinforcement::removeLastBar()
{
	reinforcement.pop_back();
	diameters.pop_back();
}

void Reinforcement::computeArea()
{
	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double tempDiameter = diameters[i];
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

void Reinforcement::rotateAroundCentroidPolygon(double angle, Point centroid)
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