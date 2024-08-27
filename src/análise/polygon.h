#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

struct Point {
    double x;
    double y;

    Point(double x_val = 0.0, double y_val = 0.0) : x(x_val), y(y_val) {}
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
    void cutByHorizontalLine(double yCut);
    void printVertices() const;
    std::vector<Point> getVertices() const;
};

class ReferencePolygon {
private:
    std::vector<Point> vertices;

    bool isClockwise() const;

public:
    void updateVertices(const std::vector<Point>& newVertices);
    void ensureCounterClockwise();
    void printVertices() const;
};

#endif // POLYGON_H
