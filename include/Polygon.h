#pragma once

#include "Point.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <stdexcept>

using std::vector;

class Polygon
{
private:
    vector<Point> polygonVertices;
    Point geometricCenter;
    Point intersectionPoint;
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
    void computeIntersection(Point firstPoint, Point secondPoint, double cutCoordY);

    vector<Point> getPolygonVertices() const;
    Point getGeometricCenter() const;
    Point getIntersectionPoint() const;
    double getMaxY() const;
    double getMinY() const;
    double getPolygonHeight() const;
    double getPolygonArea() const;
};
