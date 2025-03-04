#include "Polygon.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <stdexcept>

using std::vector;
using std::invalid_argument;

Polygon::Polygon()
{
    polygonVertices = {};
	geometricCenter;
	intersectionPoint;
	maxY = 0;
	minY = 0;
	polygonHeight = 0;
	polygonArea = 0;
	staticMomentX = 0;
	staticMomentY = 0;
}

void Polygon::setVertices(vector<Point> collectedVertices)
{
	for (size_t i = 0; i < collectedVertices.size(); i++)
	{
		double coordX = collectedVertices[i].getX();
		double coordY = collectedVertices[i].getY();
		
		polygonVertices.push_back(Point(coordX, coordY));
	}
}

void Polygon::addVertice(double coordX, double coordY)
{
	polygonVertices.push_back(Point(coordX, coordY));
}

void Polygon::removeLastVertice()
{
	polygonVertices.pop_back();
}

void Polygon::computeMaxCoordY()
{
	if (polygonVertices.empty())
		throw invalid_argument("polygonVertices is empty");

	maxY = polygonVertices[0].getY();

	for (size_t i = 0; i < polygonVertices.size(); i++)
	{
		if (polygonVertices[i].getY() > maxY) maxY = polygonVertices[i].getY();
	}
}

void Polygon::computeMinCoordY()
{
	if (polygonVertices.empty())
		throw invalid_argument("polygonVertices is empty");

	minY = polygonVertices[0].getY();

	for (size_t i = 0; i < polygonVertices.size(); i++)
	{
		if (polygonVertices[i].getY() < minY) minY = polygonVertices[i].getY();
	}
}

void Polygon::computeHeight()
{
	computeMaxCoordY();
	computeMinCoordY();

	polygonHeight = maxY - minY;
}

void Polygon::computeArea()
{
	polygonArea = 0;

	int polygonSize = polygonVertices.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		int j = (i + 1) % polygonSize;

		double factor = polygonVertices[i].getX() * polygonVertices[j].getY() 
		- polygonVertices[j].getX() * polygonVertices[i].getX();;

		polygonArea += factor;
	}

	polygonArea = abs(polygonArea) / 2; 
}

void Polygon::computeStaticMomentX()
{
	staticMomentX = 0;

	int polygonSize = polygonVertices.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		int j = (i + 1) % polygonSize;

		double factor = polygonVertices[i].getX() * polygonVertices[j].getY() 
		- polygonVertices[j].getX() * polygonVertices[i].getX();;

		staticMomentX += (polygonVertices[i].getY() + polygonVertices[j].getY()) * factor;
	}

	staticMomentX /= 6; 
}

void Polygon::computeStaticMomentY()
{
	staticMomentY = 0;

	int polygonSize = polygonVertices.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		int j = (i + 1) % polygonSize;

		double factor = polygonVertices[i].getX() * polygonVertices[j].getY() 
		- polygonVertices[j].getX() * polygonVertices[i].getX();;

		staticMomentY += (polygonVertices[i].getX() + polygonVertices[j].getX()) * factor;
	}

	staticMomentY /= 6; 
}

void Polygon::computeCentroid()
{
	computeArea();
	computeStaticMomentX();
	computeStaticMomentY();

	geometricCenter.setPoint((staticMomentX/polygonArea), (staticMomentY/polygonArea));
}

void Polygon::translateToCentroid()
{
	computeCentroid();

	for (size_t i = 0; i < polygonVertices.size(); i++)
	{
		double coordX = polygonVertices[i].getX() - geometricCenter.getX();
		double coordY = polygonVertices[i].getY() - geometricCenter.getY();

		polygonVertices[i].setPoint(coordX, coordY);
	}
}

void Polygon::rotateAroundCentroid(double angle)
{
	translateToCentroid();

	double rad = angle * M_PI / 180;
	double cosAngle = cos(rad);
	double sinAngle = sin(rad);

	for (size_t i = 0; i < polygonVertices.size(); i++)
	{
		double rotateX = (polygonVertices[i].getX() * cosAngle) - (polygonVertices[i].getY() * sinAngle);
		double rotateY = (polygonVertices[i].getX() * sinAngle) + (polygonVertices[i].getY() * cosAngle);

		polygonVertices[i].setPoint(rotateX, rotateY);
	}
}

void Polygon::computeIntersection(Point firstPoint, Point secondPoint, double cutCoordY)
{
	double dx = secondPoint.getX() - firstPoint.getX();
	double dy = secondPoint.getY() - firstPoint.getY();

	double t = (cutCoordY - firstPoint.getY()) / dy;

    intersectionPoint.setX(firstPoint.getX() + t * dx);
    intersectionPoint.setY(cutCoordY);
}

vector<Point> Polygon::getPolygonVertices() const
{
    return polygonVertices;
}

Point Polygon::getGeometricCenter() const
{
    return geometricCenter;
}

Point Polygon::getIntersectionPoint() const
{
    return intersectionPoint;
}

double Polygon::getMaxY() const
{
    return maxY;
}

double Polygon::getMinY() const
{
    return minY;
}

double Polygon::getPolygonHeight() const
{
    return polygonHeight;
}

double Polygon::getPolygonArea() const
{
    return polygonArea;
}