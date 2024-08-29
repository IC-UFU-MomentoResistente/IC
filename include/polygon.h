#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <iostream>

struct Point {
    double x;
    double y;

    Point(double x_val = 0, double y_val = 0) : x(x_val), y(y_val) {}
};

class Polygon {
private:
    std::vector<Point> vertices;

    bool isClockwise() const;

public:
    void addVertex(double x, double y);
    void ensureCounterClockwise();
    double area() const;
    Point centroid() const;
    std::vector<Point> translateToCG() const;
    void intersecao_linha_neutra(double y_neutra, double epsilon = 1e-7);
    void printVertices() const;
    std::vector<Point> getVertices() const;
};

#endif // POLYGON_H
