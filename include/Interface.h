#include <iostream>
#include <vector>
#include <string>

#include "raylib.h"
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
#include "Section.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::vector;

class Interface
{
public:
    void initInterface();
    void interfaceLoop();
    void autorsWindow();
    void showPrimaryMenuBar();
    void showSecondaryMenuBar(Section &section);
    void crossSectionData(Section &section);
    void interfaceMaterials(Section &section);
    void concreteInterface(Section &section);
    void steelInterface(Section &section);
    void reinforcementInterface(Section &section);
    void ReferenceValues();
    void effortSectionInterface(Section &section);
    void crossSectionPlotInterface(Section &section, float posY);
    void renderPolygon(const vector<Point> &polygonVertices, string nameVertices, string namePolygon);
    void renderVectorPoint(const vector<Point> &vector, string nameReinforcement);
    void renderStrainConcreteRuptureDiagram(const vector<Point> &vectorPoint, string nameVectorPoint);
    void renderStrainSteelDiagram(const vector<Point> &vectorPoint, string nameVectorPoint);
    void EffortsTable(Section &section);
    void crossSectionTable(Section &section);
    void RightTablePos(const char * nome1, const char * nome2, float posY, Section & section);
};
