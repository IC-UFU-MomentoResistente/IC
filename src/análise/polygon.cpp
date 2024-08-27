#include "polygon.h"
#include <algorithm>
#include <cmath>
#include <iostream>

bool Polygon::isClockwise() const {
    double sum = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
    }
    return sum > 0;
}

void Polygon::addVertex(double x, double y) {
    vertices.emplace_back(x, y);
}

void Polygon::ensureCounterClockwise() {
    if (isClockwise()) {
        std::reverse(vertices.begin(), vertices.end());
    }
}

double Polygon::area() const {
    double A = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        A += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
    }
    return std::abs(A) / 2.0;
}

Point Polygon::centroid() const {
    double Cx = 0.0, Cy = 0.0;
    double A = area();
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        double factor = (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
        Cx += (vertices[i].x + vertices[j].x) * factor;
        Cy += (vertices[i].y + vertices[j].y) * factor;
    }
    Cx /= (6.0 * A);
    Cy /= (6.0 * A);
    return Point(Cx, Cy);
}

std::vector<Point> Polygon::translateToCG() const {
    Point cg = centroid();
    std::vector<Point> translatedVertices;
    int n = vertices.size();

    for (size_t i = 0; i < n; ++i) {
        const Point& vertex = vertices[i];
        double translatedX = vertex.x - cg.x;
        double translatedY = vertex.y - cg.y;
        translatedVertices.emplace_back(translatedX, translatedY);
    }

    return translatedVertices;
}

void Polygon::cutByHorizontalLine(double yCut) {
    std::vector<Point> newVertices;
    int n = vertices.size();

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;

        if (vertices[i].y < yCut && vertices[j].y >= yCut) {
            // Intersection with horizontal line
            double xIntersect = vertices[i].x + (yCut - vertices[i].y) * (vertices[j].x - vertices[i].x) / (vertices[j].y - vertices[i].y);
            newVertices.emplace_back(xIntersect, yCut);
        }

        if (vertices[i].y >= yCut) {
            newVertices.emplace_back(vertices[i].x, vertices[i].y);
        }

        if (vertices[j].y >= yCut && vertices[i].y < yCut) {
            // Intersection with horizontal line
            double xIntersect = vertices[i].x + (yCut - vertices[i].y) * (vertices[j].x - vertices[i].x) / (vertices[j].y - vertices[i].y);
            newVertices.emplace_back(xIntersect, yCut);
        }
    }

    vertices = std::move(newVertices);
    ensureCounterClockwise();
}

void Polygon::printVertices() const {
    for (const auto& vertex : vertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
    }
}

std::vector<Point> Polygon::getVertices() const {
    return vertices;
}

bool ReferencePolygon::isClockwise() const {
    double sum = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
    }
    return sum > 0;
}

void ReferencePolygon::updateVertices(const std::vector<Point>& newVertices) {
    vertices = newVertices;
    ensureCounterClockwise();
}

void ReferencePolygon::ensureCounterClockwise() {
    if (isClockwise()) {
        std::reverse(vertices.begin(), vertices.end());
    }
}

void ReferencePolygon::printVertices() const {
    for (const auto& vertex : vertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
    }
}
