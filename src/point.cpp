#include "point.h"

// Construtor padrão
Point::Point() : x(0), y(0) {}

// Construtor com parâmetros
Point::Point(float x_val, float y_val) : x(x_val), y(y_val) {}

// Operador de comparação de igualdade
bool Point::operator==(const Point &other) const {
    const float epsilon = 1e-5;
    return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon;
}

// Operador de comparação de desigualdade
bool Point::operator!=(const Point &other) const {
    return !(*this == other);
}

// Operador de comparação para ordenação
bool Point::operator<(const Point &other) const {
    if (x != other.x) {
        return x < other.x;
    }
    return y < other.y;
}