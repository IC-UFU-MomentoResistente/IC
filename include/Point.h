#pragma once

#include <vector>

using std::vector;

class Point
{
private:
    double x;
    double y;

public:
    Point();
    Point(double coordX, double coordY);

    double getX() const;
    double getY() const;
    Point getPoint() const;

    double setX(double coordX);
    double setY(double coordY);
    Point setPoint(double coordX, double coordY); 
};