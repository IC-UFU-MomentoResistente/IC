#include "Reinforcement.h"

Reinforcement::Reinforcement()
{
	reinforcement = {};
	diameters = {};
	areas = {};
	effectiveDepth = 0;
}

void Reinforcement::setReinforcement(vector<Point> collectedReinforcement, vector<double> collectedDiameters)
{
	for (size_t i = 0; i < collectedReinforcement.size(); i++)
	{
		double coordX = collectedReinforcement[i].getX();
		double coordY = collectedReinforcement[i].getY();

		reinforcement.push_back(Point(coordX, coordY));

		double diameter = collectedDiameters[i]; // mm

		diameters.push_back(diameter);
	}
}

void Reinforcement::addReinforcement(double coordX, double coordY, double diameter)
{
	reinforcement.push_back(Point(coordX, coordY));
	diameters.push_back(diameter);
}

void Reinforcement::removeLastReinforcement()
{
	reinforcement.pop_back();
	diameters.pop_back();
	areas.pop_back();
}

void Reinforcement::clearReinforcement()
{
	reinforcement.clear();
	diameters.clear();
	areas.clear();
}

void Reinforcement::computeArea()
{
	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double tempDiameter = diameters[i] / 10; // cm
		double tempArea = pow(tempDiameter, 2) * 3.141592653589793 / 4;
		areas.push_back(tempArea);
	}
}

void Reinforcement::translateToCentroidPolygon(Point centroid)
{
	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double coordX = reinforcement[i].getX() - centroid.getX();
		double coordY = reinforcement[i].getY() - centroid.getY();

		reinforcement[i].setX(coordX);
		reinforcement[i].setY(coordY);
	}
}

void Reinforcement::rotateAroundCentroidPolygon(double angle)
{
	double rad = angle * 3.14159265358979323846 / 180;
	double cosAngle = cos(rad);
	double sinAngle = sin(rad);

	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		double rotateX = (reinforcement[i].getX() * cosAngle) - (reinforcement[i].getY() * sinAngle);
		double rotateY = (reinforcement[i].getX() * sinAngle) + (reinforcement[i].getY() * cosAngle);
		
		reinforcement[i].setX(rotateX);
		reinforcement[i].setY(rotateY);
	}
}

void Reinforcement::setEffectiveDepth(double maxY)
{
	double temp = 0;

	for (size_t i = 0; i < reinforcement.size(); i++)
	{
		if (reinforcement[i].getY() <= temp)
			temp = reinforcement[i].getY();
	}

	if (maxY > temp)
	{
		effectiveDepth = maxY - temp; // cm
	}
	else
	{
		effectiveDepth = 0;
	}
}

vector<Point> Reinforcement::getReinforcement() const
{
	return reinforcement;
}

vector<double> Reinforcement::getDiameters() const
{
	return diameters;
}

vector<double> Reinforcement::getAreas() const
{
	return areas;
}

double Reinforcement::getEffectiveDepth() const
{
    return effectiveDepth;
}

void Reinforcement::SetNumPoints(int numPointsInput)
{
    if (numPointsInput < 0) numPointsInput = 0;
    reinforcement.resize(numPointsInput);  // Ajusta o tamanho do vetor de vértices
	diameters.resize(numPointsInput);  // Ajusta o tamanho do vetor de diâmetros
}

int Reinforcement::GetNumPoints() const
{
    return reinforcement.size();  // Retorna o número de pontos
}

void Reinforcement::GetTableData(int index, double* x, double* y, double* d) const
{
    if (index < 0 || index >= GetNumPoints()) return;
    *x = reinforcement[index].getX();  // Obtém a coordenada X do vértice
    *y = reinforcement[index].getY();  // Obtém a coordenada Y do vértice
	*d = diameters[index];  // Obtém o diâmetro do vértice
}

void Reinforcement::SetTableData(int index, double x, double y, double d)
{
    if (index < 0 || index >= GetNumPoints()) return;
    reinforcement[index].setX(x);  // Define a coordenada X do vértice
    reinforcement[index].setY(y);  // Define a coordenada Y do vértice
	diameters[index] = d;  // Define o diâmetro do vértice
}
