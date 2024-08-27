#pragma once
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"
#include "data_storage.h"  // Inclui a biblioteca de armazenamento de dados
#include <stdio.h>
#include <polygon.h>

void updatePolygonFromInterface();
void iniciarInterface();
void iniciarLoop();
void limpeza();