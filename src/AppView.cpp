#include "Point.h"
#include "AppView.h"

#include "raylib.h"
#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

void AppView::renderPolygon(const vector<Point>& polygonVertices, string nameVertices, string namePolygon)
{
	if (!polygonVertices.empty())
	{
		vector<double> xTemp(polygonVertices.size());
		vector<double> yTemp(polygonVertices.size());

		for (size_t i = 0; i < polygonVertices.size(); i++)
		{
			xTemp[i] = polygonVertices[i].getX();
			yTemp[i] = polygonVertices[i].getY();
		}

		ImPlot::PlotScatter(nameVertices.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(polygonVertices.size()));

		if (polygonVertices.size() > 2)
		{
			vector<double> xTempEdge(polygonVertices.size() + 1);
			vector<double> yTempEdge(polygonVertices.size() + 1);

			for (size_t i = 0; i < polygonVertices.size(); i++)
			{
				xTempEdge[i] = polygonVertices[i].getX();
				yTempEdge[i] = polygonVertices[i].getY();
			}

			xTempEdge[polygonVertices.size()] = polygonVertices[0].getX();
			yTempEdge[polygonVertices.size()] = polygonVertices[0].getY();

			ImPlot::PlotLine(namePolygon.c_str(), xTempEdge.data(), yTempEdge.data(), static_cast<int>(xTempEdge.size()));
		}
	}
}

void AppView::renderReinforcement(const vector<Point>& reinforcement, string nameReinforcement)
{
	if (!reinforcement.empty())
	{
		vector<double> xTemp(reinforcement.size());
		vector<double> yTemp(reinforcement.size());

		for (size_t i = 0; i < reinforcement.size(); i++)
		{
			xTemp[i] = reinforcement[i].getX();
			yTemp[i] = reinforcement[i].getY();
		}

		ImPlot::PlotScatter(nameReinforcement.c_str(), xTemp.data(), yTemp.data(), static_cast<int>(reinforcement.size()));
	}
}