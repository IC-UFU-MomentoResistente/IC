#pragma once

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"
#include "controller.h"

class View
{
public:
    static void iniciar();
    static void renderizar(Controller &controller);
};
