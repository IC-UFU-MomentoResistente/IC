#ifndef POLIGONO_H
#define POLIGONO_H

#include <vector>
#include <set>
#include <cmath>
#include "point.h"

#define _USE_MATH_DEFINES

// Definição da classe Poligono
class Poligono {
public:
    std::vector<Point> vertices;
    std::vector<Point> resultadoCorte;
    std::vector<Point> areaSuperior;
    std::vector<Point> areaInferior;
    std::vector<Point> verticesTransladados;
    std::vector<Point> verticesRotacionados;

    void setVertices(const std::vector<Point>& points);
    double area() const;
    Point centroide() const;
    Point calcularCentroid(const std::vector<Point>& pontos);
    void ordenarPontosAntihorario(std::vector<Point>& pontos);
    void translacaoCG(const std::vector<Point>& points);
    void rotacao(double angulo);
    void MaxMin(float& yMax, float& yMin) const;
    int verificarCaso(const Point& p1, const Point& p2, double cortar);
    Point calcularIntersecao(const Point& p1, const Point& p2, double cortar);
    void cortarPoligonal(const std::vector<Point>& verticesTransladados, float& cortar);
    void fecharPoligono(std::vector<Point> &pontos);
    const std::vector<Point> getVertices () const;
    const std::vector<Point> getResultadoCorte () const;
    const std::vector<Point> getAreaSuperior () const;
    const std::vector<Point> getAreaInferior () const;
    const std::vector<Point> getVerticesTransladados () const;
    const std::vector<Point> getVerticesRotacionados () const;
};

#endif // POLIGONO_H
