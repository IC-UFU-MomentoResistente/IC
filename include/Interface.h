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
#include "MomentSolver.h"
#include "Section.h"

#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;

class Interface
{
private:
    bool shouldAutoFit = false;

public:
    void initInterface();
    void interfaceLoop();
    void autorsWindow();
    void showPrimaryMenuBar(Section &section);
    void showSecondaryMenuBar(Section &section);
    void crossSectionData(Section &section);
    void inputSectionRectangle(Section &section);
    void inputSectionT(Section &section);
    void inputSectionCircular(Section &section);
    void inputSectionPolygonal(Section &section);
    void inputSectionDebug(Section &section);
    void showGeometricParameters(Section &section);
    void showPopUpErrorPolygon();
    void clearInputSection(Section &section);
    void clearSection(Section &section);
    void interfaceMaterials(Section &section);
    void concreteInterface(Section &section);
    void steelInterface(Section &section);
    void reinforcementInterface(Section &section);
    void ReferenceValues();
    void effortSectionInterface(Section &section);
    void crossSectionPlotInterface(Section &section, float posY);
    void envelopeMomentsPlotInterface(Section &section, float posY);
    void renderPolygon(const vector<Point> &polygonVertices, string nameVertices, string namePolygon);
    void renderVectorPoint(const vector<Point> &vector, string nameReinforcement);
    void renderStrainConcreteRuptureDiagram(const vector<Point> &vectorPoint, string nameVectorPoint);
    void renderStrainSteelDiagram(const vector<Point> &vectorPoint, string nameVectorPoint);
    void EffortsTable(Section &section);
    void crossSectionTable(Section &section);
    void RightTablePos(const char *nome1, const char *nome2, float posY, Section &section);
    void applyDarkElegantPlotStyle();

    void saveSectionData(Section &section, const std::string &filename);
    void loadSectionData(Section &section, const std::string &filename);
    void autoFitToPointsWithMargin(const vector<Point> &points, float margin = 0.1f);
};
