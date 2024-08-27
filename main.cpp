#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"
#include "data_storage.h"  // Inclui a biblioteca de armazenamento de dados
#include <stdio.h>
#include "window.h"

int main() {
    // Inicializa a janela do Raylib
   
   iniciarInterface();  
   iniciarLoop();
   limpeza();

    return 0;
}
