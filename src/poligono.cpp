#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <algorithm>
#include "point.h"
#include "poligono.h"

// Implementação do método setVertices
void Poligono::setVertices(const std::vector<Point>& points) {
    vertices.clear();
    for (const auto& p : points) {
        vertices.emplace_back(p.x, p.y);
    }
}

// Implementação do método area
double Poligono::area() const {
    double A = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        A += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
    }
    return std::abs(A) / 2.0;
}

// Implementação do método centroide
Point Poligono::centroide() const {
    double Cx = 0.0, Cy = 0.0;
    double A = area();
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        double factor = (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
        Cx += (vertices[i].x + vertices[j].x) * factor;
        Cy += (vertices[i].y + vertices[j].y) * factor;
    }
    Cx /= (6.0 * A);
    Cy /= (6.0 * A);
    return Point(Cx, Cy);
}

// Função para calcular o centróide
Point Poligono::calcularCentroid(const std::vector<Point>& pontos) {
    float cx = 0, cy = 0;
    for (const auto& ponto : pontos) {
        cx += ponto.x;
        cy += ponto.y;
    }
    cx /= pontos.size();
    cy /= pontos.size();
    return {cx, cy};
}

// Função para ordenar os pontos em sentido anti-horário
void Poligono::ordenarPontosAntihorario(std::vector<Point>& pontos) {
    Point centroid = calcularCentroid(pontos);
    std::sort(pontos.begin(), pontos.end(), [&centroid](const Point& a, const Point& b) {
        float angA = atan2(a.y - centroid.y, a.x - centroid.x);
        float angB = atan2(b.y - centroid.y, b.x - centroid.x);
        return angA < angB;
    });
}

// Implementação do método translacaoCG
void Poligono::translacaoCG(const std::vector<Point>& points) {
    verticesTransladados.clear();
    Point cg = centroide();

    for (const auto& p : points) {
        float u = p.x - cg.x;
        float v = p.y - cg.y;
        verticesTransladados.emplace_back(u, v);
    }

    ordenarPontosAntihorario(verticesTransladados);
}

// Implementação do método rotacao
void Poligono::rotacao(double angulo) {
    verticesRotacionados.clear();
    double ang_radianos = angulo * M_PI / 180.0;

    for (const auto& p : verticesTransladados) {
        float u = (p.x * cos(ang_radianos)) - (p.y * sin(ang_radianos));
        float v = (p.x * sin(ang_radianos)) + (p.y * cos(ang_radianos));
        verticesRotacionados.emplace_back(u, v);
    }

    ordenarPontosAntihorario(verticesRotacionados);
}

// Implementação do método MaxMin
void Poligono::MaxMin(float& yMin, float& yMax) const {
    if (verticesTransladados.empty()) return;

    yMin = verticesTransladados[0].y;
    yMax = verticesTransladados[0].y;

    for (const auto& p : verticesTransladados) {
        if (p.y < yMin) yMin = p.y;
        if (p.y > yMax) yMax = p.y;
    }
}

// Implementação do método verificarCaso
int Poligono::verificarCaso(const Point& p1, const Point& p2, double cortar) {
    if ((p2.y <= cortar && p1.y >= cortar) || (p2.y >= cortar && p1.y <= cortar)){
        return 2; // Caso 2: Corta o trecho
    }
    if ((p2.y >= cortar && p1.y >= cortar) || (p1.y >= cortar) || (p2.y >= cortar)){
        return 3; // caso 3: vértices acima da linha de corte; área comprimida
    }
    if ((p2.y <= cortar && p1.y <= cortar) || (p1.y <= cortar) || (p2.y <= cortar)){
        return 4; // caso 4: vértices abaixo da linha de corte: área tracionada
    }
    return 1; // Caso 1: Adiciona o nó final na nova poligonal
}

// Implementação do método calcularIntersecao
Point Poligono::calcularIntersecao(const Point& p1, const Point& p2, double cortar) {
    double deltaY = p2.y - p1.y;
    double deltaX = p2.x - p1.x;

    if (std::abs(deltaX) < 1e-6) {
        return Point(p1.x, cortar);
    } else {
        double tg = deltaY / deltaX;
        double x = p1.x - (p1.y - cortar) / tg;
        return Point(x, cortar);
    }
}

// Implementação do método cortarPoligonal
void Poligono::cortarPoligonal(const std::vector<Point>& verticesCorte, float& cortar) {
    resultadoCorte.clear();
    areaSuperior.clear();
    areaInferior.clear();
    int nv = verticesCorte.size();
    std::set<Point> pontosAdicionados;

    for (int i = 0; i < nv; i++)
    {
        const Point &p1 = verticesCorte[i];
        const Point &p2 = verticesCorte[(i + 1) % nv];

        int caso = verificarCaso(p1, p2, cortar);

        if (caso == 2) 
        {
            Point intersecao = calcularIntersecao(p1, p2, cortar);
            if (pontosAdicionados.insert(intersecao).second)
            {
                resultadoCorte.push_back(intersecao);
                areaSuperior.push_back(intersecao);
                areaInferior.push_back(intersecao);
            }
            
            if(pontosAdicionados.insert(p1).second)
            {
                if (p1.y > cortar)
                {
                    areaSuperior.push_back(p1);
                    resultadoCorte.push_back(p1);
                }
                else
                {
                    areaInferior.push_back(p1);
                    resultadoCorte.push_back(p1);
                }
            }

            if (pontosAdicionados.insert(p2).second)
            {
                if (p2.y > cortar)
                {
                    areaSuperior.push_back(p2);
                    resultadoCorte.push_back(p2);
                }
                else
                {
                    areaInferior.push_back(p2);
                    resultadoCorte.push_back(p2);
                }
            }
        }
        else if (caso == 3) 
        {
            if (pontosAdicionados.insert(p1).second)
            {
                resultadoCorte.push_back(p1);
                areaSuperior.push_back(p1);
            }
            if (pontosAdicionados.insert(p2).second)
            {
                resultadoCorte.push_back(p2);
                areaSuperior.push_back(p2);
            }
        }
        else if (caso == 4) 
        {
            if (pontosAdicionados.insert(p1).second)
            {
                resultadoCorte.push_back(p1);
                areaInferior.push_back(p1);
            }
            if (pontosAdicionados.insert(p2).second)
            {
                resultadoCorte.push_back(p2);
                areaInferior.push_back(p2);
            }
        }
    }

    ordenarPontosAntihorario(resultadoCorte);
    ordenarPontosAntihorario(areaSuperior);
    ordenarPontosAntihorario(areaInferior);
}

void Poligono::fecharPoligono(std::vector<Point> &pontos) {
    if (!pontos.empty())
    {
        pontos.push_back(pontos[0]);
    }
}