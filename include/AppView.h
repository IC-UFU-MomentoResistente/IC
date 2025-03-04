#pragma once

#include "Point.h"

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

class AppView
{
public:
    void renderPolygon(const vector<Point>& polygonVertices, string nameVertices, string namePolygon);
};
