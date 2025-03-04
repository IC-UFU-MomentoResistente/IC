#include "AppController.h"

#include "Point.h"
#include "Polygon.h"

#include <vector>

using std::vector;

void AppController::addVertice(const double& coordX, const double& coordY)
{
    polygon.addVertice(coordX, coordY);
}

void AppController::removeLastVertice()
{
    polygon.removeLastVertice();
}

vector<Point> AppController::getPolygonVertices()
{
    return polygon.getPolygonVertices();
}
