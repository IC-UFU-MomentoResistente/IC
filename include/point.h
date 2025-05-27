#pragma once

#include "cereal/cereal.hpp"

// Forward declaration para serializar Point diretamente
namespace cereal
{
    template <class Archive> void serialize(Archive& ar, struct Point& point);
}

struct Point
{
    double x;
    double y;

    Point(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    // Serialização/desserialização para a struct 
    template <class Archive>
    void serialize (Archive& archive)
    {
        archive(CEREAL_NVP(x), CEREAL_NVP(y)); //NVP = Name-Value Pair
    }
};