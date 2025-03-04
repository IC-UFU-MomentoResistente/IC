#pragma once

#include "Point.h"
#include "Polygon.h"

#include <vector>

using std::vector;

class AppController
{
private:
    Polygon polygon;

public:
    void addVertice(const double& coordX, const double& coordY);
    void removeLastVertice();
    vector<Point> getPolygonVertices();
};