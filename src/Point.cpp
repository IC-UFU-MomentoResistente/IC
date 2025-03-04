#include "Point.h"

Point::Point()
{
    x = 0;
    y = 0;
}

Point::Point(double coordX, double coordY)
{
    x = coordX;
    y = coordY;
}

double Point::getX() const { return x; }

double Point::getY() const { return y; }

double Point::setX(double coordX) { return x = coordX; }

double Point::setY(double coordY) { return y = coordY; }