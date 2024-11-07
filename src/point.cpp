#include "point.h"

// Implementação do construtor padrão de Point
Point::Point() : x(0), y(0) {}

// Implementação do construtor com parâmetros de Point
Point::Point(float x_val, float y_val) : x(x_val), y(y_val) {}

// Implementação do operador de comparação para Point
bool Point::operator<(const Point &other) const
{
    if (x != other.x)
    {
        return x < other.x;
    }
    return y < other.y;
}