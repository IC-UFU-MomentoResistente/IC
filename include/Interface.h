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
    void showSecondaryMenuBar(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &ConcreteProperties, AppView &view);
    void crossSectionData(Polygon &polygon, Reinforcement &reinforcement);
    void interfaceMaterials(ConcreteProperties &concrete, AppView &view);
    void reinforcementInterface(Reinforcement &reinforcement);
    void effortSectionInterface();
    void crossSectionPlotInterface(AppView & view, Polygon & polygon, Reinforcement & reinforcement);
};
