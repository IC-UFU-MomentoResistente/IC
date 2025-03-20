#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

class Interface 
{



public:
    void InitInterface();
    void InterfaceLoop();
    void AutorsWindow();
    void ShowPrimaryMenuBar();
    void ShowSecondaryMenuBar(Polygon &polygon, Reinforcement &reinforcement, ConcreteProperties &ConcreteProperties, AppView &view);
    void CrossSectionData(Polygon &polygon, Reinforcement &reinforcement);
    void InterfaceMaterials(ConcreteProperties &concrete, AppView &view);
    void ReinforcementInterface(Reinforcement &reinforcement);
    void EffortSectionInterface();
    void crossSectionPlotInterface(AppView & view, Polygon & polygon, Reinforcement & reinforcement);
};
