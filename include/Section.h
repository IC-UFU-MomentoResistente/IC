#pragma once

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "SteelProperties.h"
#include "Combination.h"

class Section
{
public:
    Point point;
    Polygon polygon;
    Reinforcement reinforcement;
    ConcreteProperties concrete;
    SteelProperties steel;
    std::vector<Combination> combinations;
    Section();
};
