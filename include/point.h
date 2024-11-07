#ifndef POINT_H
#define POINT_H

struct Point
{
    float x;
    float y;

    Point();                         // Construtor padrão
    Point(float x_val, float y_val); // Construtor com parâmetros

    // Operador de comparação para que o std::set funcione
    bool operator<(const Point &other) const;
};

#endif // POINT_H