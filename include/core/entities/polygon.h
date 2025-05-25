#pragma once 

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include "point.h"
#include <vector>
#include <string>
#include <cstdint>

namespace cereal
{
    template <class Archive> void serialize(Archive& ar, class Polygon& polygon);
}

class Polygon
{
public:
    uint64_t id;
    std::string name;
    std::vector<Point> vertices;
    
    // Construtor padrão para cereal
    Polygon();

    // Construtor com parâmetros
    Polygon(uint64_t id, std::string name, const std::vector<Point>& vertices);

    // Métodos 
    void addVertice (const Point& point);
    void addVertice (double x, double y);
    size_t getNumVertices() const;
    bool isValid() const;
    
    // Serialização/desserialização para a classe
    template <class Archive>
    void serialize (Archive& archive)
    {
        archive(CEREAL_NVP(id), CEREAL_NVP(vertices));
    }
};