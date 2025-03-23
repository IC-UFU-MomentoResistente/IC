#include "Section.h"

Section::Section()
{
    point = Point();
    polygon = Polygon();
    reinforcement = Reinforcement();
    concrete = ConcreteProperties();
    steel = SteelProperties();
    combinations = std::vector<Combination>();
}