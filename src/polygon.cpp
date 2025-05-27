#include "polygon.h"

Polygon::Polygon() : id(0), name("Indefinido") {}

Polygon::Polygon(uint64_t id, std::string name, const std::vector<Point>& vertices) 
    : id(id), name(std::move(name)), vertices(vertices) {}

void Polygon::addVertice(const Point& point) { vertices.push_back(point); }

void Polygon::addVertice(double x, double y) { vertices.emplace_back(x, y); }

size_t Polygon::getNumVertices() const { return vertices.size(); }

bool Polygon::isValid() const { return vertices.size() >= 3; }

