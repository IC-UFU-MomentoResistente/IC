#include "Section.h"

Section::Section()
{
    point = Point();
    polygon = Polygon();
    reinforcement = Reinforcement();
    concrete = ConcreteProperties();
    steel = SteelProperties();
    strainDistribution = StrainDistribution();
    stressRegions = PolygonStressRegions();
}