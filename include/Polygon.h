#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <stdexcept>

#include "Point.h"

using std::invalid_argument;
using std::vector;

class Polygon
{
private:
	vector<Point> polygonVertices;
	Point geometricCenter;
	double maxY;
	double minY;
	double polygonHeight;
	double polygonArea;
	double staticMomentX;
	double staticMomentY;

public:
	Polygon();

	void setVertices(vector<Point> collectedVertices);
	void addVertice(double coordX, double coordY);
	void removeLastVertice();
	void computeMaxCoordY();
	void computeMinCoordY();
	void computeHeight();
	void computeArea();
	void computeStaticMomentX();
	void computeStaticMomentY();
	void computeCentroid();
	void translateToCentroid();
	void rotateAroundCentroid(double angle);

	vector<Point> getPolygonVertices() const;
	Point getGeometricCenter() const;
	double getMaxY() const;
	double getMinY() const;
	double getPolygonHeight() const;
	double getPolygonArea() const;
	double getVet0X() const;
	double getVet0Y() const;
};