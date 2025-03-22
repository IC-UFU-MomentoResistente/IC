#pragma once

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"

class Section
{
public:
    Point point;
    Polygon polygon;
    Reinforcement reinforcement;
    ConcreteProperties concrete;

    Section();
};

