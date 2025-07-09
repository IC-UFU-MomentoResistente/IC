#include <fstream>
#include <iostream>

// Headers do Emscripten para o loop principal
#include <emscripten/emscripten.h>

// Headers da sua aplicação
#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"
#include "Section.h"
#include "Interface.h"

// Estrutura para manter o estado da nossa aplicação vivo entre os frames
struct AppContext {
    Section section;
    Interface interface;
};

// Esta função será chamada pelo navegador repetidamente (ex: 60 vezes por segundo)
void main_loop(void* arg) {
    AppContext* ctx = static_cast<AppContext*>(arg);

    // --- TODO O CÓDIGO QUE ESTAVA DENTRO DO SEU 'while' VEM AQUI ---
    BeginDrawing();
    ClearBackground(DARKGRAY);
    rlImGuiBegin();

    ctx->interface.showPrimaryMenuBar(ctx->section);
    ctx->interface.showSecondaryMenuBar(ctx->section);
    ctx->interface.crossSectionPlotInterface(ctx->section, 56);
    ctx->interface.envelopeMomentsPlotInterface(ctx->section, 56);
    ctx->interface.RightTablePos("Tabela de Pontos", "Tabela de Esforços", 56, ctx->section);

    rlImGuiEnd();
    EndDrawing();
    // --- FIM DO CÓDIGO DO LOOP ---
}

int main() {
    // A inicialização continua a mesma
    // Não precisamos mais do log em arquivo, pois o console do navegador será nosso log
    
    AppContext ctx;
    ctx.interface.initInterface(); // Sua função que chama InitWindow e outras inicializações

    // Em vez de um laço while, nós registramos a função de loop com o Emscripten
    // O navegador se encarregará de chamá-la continuamente.
    // O '0' significa para rodar na maior frequência possível (geralmente limitado pelo navegador)
    // O '1' significa para simular um loop infinito.
    emscripten_set_main_loop_arg(main_loop, &ctx, 0, 1);

    // O código abaixo só será alcançado se o loop for cancelado
    ImPlot::DestroyContext();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}