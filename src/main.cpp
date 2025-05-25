#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

int main()
{
	InitWindow(800, 600, "Calculadora Raylib");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Olá!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
    CloseWindow();
}