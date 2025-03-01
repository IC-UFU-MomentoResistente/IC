#include "view.h"

void View::iniciar()
{
    int screenWidth = 1280;
    int screenHeight = 800;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "IC - FCO - Momentos Resistentes");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
}

void View::renderizar(Controller &controller)
{
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // start ImGui Conent
        rlImGuiBegin();

        bool janelaSecao = true;
        if (janelaSecao)
        {
            // inicia a janela no ImGui
            ImGui::Begin("Secao Transversal", &janelaSecao);

            static double fck = 30, fyk = 500, bw = 30, d = 50, As = 5;

            ImGui::InputDouble("fck (MPa)", &fck);
            ImGui::InputDouble("fyk (MPa)", &fyk);
            ImGui::InputDouble("bw (cm)", &bw);
            ImGui::InputDouble("d (cm)", &d);
            ImGui::InputDouble("As (cm^2)", &As);

            if(ImGui::Button("Calcular Momento Resistente"))
            {
                controller.setDados(fck, fyk, bw, d, As);
                controller.calcularMonento();
            }

            double momento = controller.getMomento();
            ImGui::Text("Momento Resistente: %.2f kN.m", momento);

            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            // inicialização do gráfico com os eixos
            if (ImPlot::BeginPlot("Grafico", ImVec2(plotSize.x, plotSize.y), ImPlotFlags_Equal))
            {
        
            }

            ImPlot::EndPlot();

            // finaliza a janela no ImGui
            ImGui::End();
        }
        // end ImGui Content
        rlImGuiEnd();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    ImPlot::DestroyContext();
    rlImGuiShutdown();

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}
