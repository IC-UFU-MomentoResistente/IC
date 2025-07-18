// #include <fstream>
// #include <iostream>

// // Headers do Emscripten para o loop principal
// #include <emscripten/emscripten.h>

// // Headers da sua aplicação
// #include "raylib.h"
// #include "imgui.h"
// #include "implot.h"
// #include "rlImGui.h"
// #include "Section.h"
// #include "Interface.h"

// EM_JS(double, get_device_pixel_ratio, (), 
// {
//     return window.devicePixelRatio || 1;
// })

// // Estrutura para manter o estado da nossa aplicação vivo entre os frames
// struct AppContext {
//     Section section;
//     Interface interface;
// };

// // Esta função será chamada pelo navegador repetidamente (ex: 60 vezes por segundo)
// void main_loop(void* arg) {
//     AppContext* ctx = static_cast<AppContext*>(arg);

//     float pixel_ratio = (float)get_device_pixel_ratio();

//     Vector2 mouse_pos = GetMousePosition();

//     ImGuiIO& io = ImGui::GetIO();

//     io.DisplayFramebufferScale = ImVec2(pixel_ratio, pixel_ratio);

//     io.DisplaySize = ImVec2((float)GetScreenWidth() / pixel_ratio, (float)GetScreenHeight() / pixel_ratio);

//     // --- TODO O CÓDIGO QUE ESTAVA DENTRO DO SEU 'while' VEM AQUI ---
//     BeginDrawing();
//     ClearBackground(DARKGRAY);
//     rlImGuiBegin();

//     ctx->interface.showPrimaryMenuBar(ctx->section);
//     ctx->interface.showSecondaryMenuBar(ctx->section);
//     ctx->interface.crossSectionPlotInterface(ctx->section, 56);
//     ctx->interface.envelopeMomentsPlotInterface(ctx->section, 56);
//     ctx->interface.RightTablePos("Tabela de Pontos", "Tabela de Esforços", 56, ctx->section);

//     rlImGuiEnd();
//     EndDrawing();
//     // --- FIM DO CÓDIGO DO LOOP ---
// }

// int main() {
//     // A inicialização continua a mesma
//     // Não precisamos mais do log em arquivo, pois o console do navegador será nosso log
    
//     AppContext ctx;
//     ctx.interface.initInterface(); // Sua função que chama InitWindow e outras inicializações

//     // Em vez de um laço while, nós registramos a função de loop com o Emscripten
//     // O navegador se encarregará de chamá-la continuamente.
//     // O '0' significa para rodar na maior frequência possível (geralmente limitado pelo navegador)
//     // O '1' significa para simular um loop infinito.
//     emscripten_set_main_loop_arg(main_loop, &ctx, 0, 1);

//     // O código abaixo só será alcançado se o loop for cancelado
//     ImPlot::DestroyContext();
//     rlImGuiShutdown();
//     CloseWindow();

//     return 0;
// }

#include <fstream>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include "Point.h"
#include "Polygon.h"
#include "Reinforcement.h"
#include "ConcreteProperties.h"
#include "SteelProperties.h"
#include "StrainDistribution.h"
#include "PolygonStressRegions.h"
#include "AnalyticalIntegration.h"
#include "InternalForces.h"
#include "MomentSolver.h"
#include "Section.h"
#include "Interface.h"
#include "combination.h"
#include "segoeuisl_data.h"

using std::cout;
using std::endl;
using std::vector;

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

int main()
{
	Section section;
	Interface interface;

	int screenWidth = 1280;
    int screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "SOFTWARE DE CÁLCULO DO MOMENTO RESISTENTE EM SEÇÕES DE CONCRETO ARMADO");
    SetWindowMinSize(320,240);

    // if (!IsWindowReady()) // Verifique se a janela foi criada com sucesso
    // {
    //     std::cerr << "Erro ao criar a janela!" << std::endl;
    //     return; // Saia da função se a janela não foi criada
    // }

    SetTargetFPS(60);

    rlImGuiBeginInitImGui();

    ImGui::StyleColorsDark();

    ImFontConfig fontConfig;
    static const ImWchar customRange[] = {
        0x0020, 0x00FF, // ASCII estendido (contém ², ³)
        0x0370, 0x03FF, // Grego
        0x2070, 0x209F, // Sobrescritos e Subscritos (contém ⁰¹²³⁴⁵⁶⁷⁸⁹)
        0x2030, 0x2030, // Símbolo de por mil (‰)
        0};

    ImGuiIO &io = ImGui::GetIO();
    // ImFont *customFont = io.Fonts->AddFontFromFileTTF("src/segoeuisl.ttf", 18.0f, &fontConfig, customRange);

    ImFont *customFont = io.Fonts->AddFontFromMemoryTTF(
        (void *)segoeuisl_ttf,
        segoeuisl_ttf_len,
        18.0f,
        &fontConfig,
        customRange);

    if (customFont)
    {
        io.FontDefault = customFont;
    }
    else
    {
        std::cerr << "Erro ao carregar a fonte personalizada!" << std::endl;
    }

    rlImGuiEndInitImGui();
    ImPlot::CreateContext();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);
		rlImGuiBegin();

        float scale = MIN((float)GetScreenWidth()/screenWidth, (float)GetScreenHeight()/screenHeight);

        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (screenWidth*scale))*0.5f)/scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (screenHeight*scale))*0.5f)/scale;
        virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)screenWidth, (float)screenHeight });

        // Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
        //SetMouseOffset(-(GetScreenWidth() - (gameScreenWidth*scale))*0.5f, -(GetScreenHeight() - (gameScreenHeight*scale))*0.5f);
        //SetMouseScale(1/scale, 1/scale);

		interface.showPrimaryMenuBar(section);
		interface.showSecondaryMenuBar(section);
		interface.crossSectionPlotInterface(section, 56);
		interface.envelopeMomentsPlotInterface(section, 56);
		interface.RightTablePos("Tabela de Pontos", "Tabela de Esforços", 56, section);

		// bool showDemoWindow = true;
		// if (showDemoWindow)
		// 	ImGui::ShowDemoWindow(&showDemoWindow);

		rlImGuiEnd();
		EndDrawing();
	}

	ImPlot::DestroyContext();
	rlImGuiShutdown();
	CloseWindow();

	return 0;
}