#pragma once

#include "point.h"
#include <vector>
#include <string>

struct PolygonDto
{
    std::string name;
    std::vector<Point> vertices;
};