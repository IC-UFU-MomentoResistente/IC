#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

class Interface 
{
public:
    void initInterface();
    void interfaceLoop();
    void autorsWindow();
    void showPrimaryMenuBar();
    void showSecondaryMenuBar(Section &section, AppView &view);
    void crossSectionData(Section &section);
    void interfaceMaterials(Section &section, AppView &view);
    void reinforcementInterface(Section &section);
    void effortSectionInterface();
    void crossSectionPlotInterface(Section &section, AppView & view);
};
