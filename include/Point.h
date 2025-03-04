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

    double setX(double coordX);
    double setY(double coordY);
};