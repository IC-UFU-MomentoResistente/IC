#include "Polygon.h"
#include <vector>
#include <emscripten/emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE 
double calcularAreaRetangulo(float base, float altura)
{
    Polygon retangulo;

    std::vector<Point> vertices = {
        {0, 0},
        {base, 0},
        {base, altura},
        {0, altura}
    };

    retangulo.setVertices(vertices);

    retangulo.computeArea();

    return retangulo.getPolygonArea();
}

}