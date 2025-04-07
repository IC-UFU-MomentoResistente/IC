#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>

#include "Point.h"

using std::vector;

class Reinforcement
{
private:
	vector<Point> reinforcement;
	vector<double> diameters;
	vector<double> areas;

public:
	Reinforcement();

	void setReinforcement(vector<Point> collectedReinforcement, vector<double> collectedDiameters);
	void addReinforcement(double coordX, double coordY, double diameter);
	void removeLastReinforcement();
	void removeLastBar();
	void clearReinforcement();
	void computeArea();
	void translateToCentroidPolygon(Point centroid);
	void rotateAroundCentroidPolygon(double angle, Point centroid);

	vector<Point> getReinforcement() const;
	vector<double> getDiameters() const;
	vector<double> getAreas() const;
    void SetNumPoints(int numPointsInput);
    int GetNumPoints() const;
    void GetTableData(int index, double *x, double *y, double *d) const;
    void SetTableData(int index, double x, double y, double d);
};