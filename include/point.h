#ifndef POINT_H
#define POINT_H

#include <cmath>

class Point {
public:
    float x, y;

    // Construtor padrão
    Point();

    // Construtor com parâmetros
    Point(float x_val, float y_val);

    // Operador de comparação de igualdade
    bool operator==(const Point &other) const;

    // Operador de comparação de desigualdade
    bool operator!=(const Point &other) const;

    // Operador de comparação para ordenação
    bool operator<(const Point &other) const;
};

#endif // POINT_H