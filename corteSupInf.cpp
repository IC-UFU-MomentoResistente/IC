#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <set>

struct Point {
    float x;
    float y;

    Point() : x(0), y(0) {}
    Point(float x_val, float y_val) : x(x_val), y(y_val) {}

    // Definir o operador de comparação para que o std::set funcione
    bool operator<(const Point& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }
};

class Polygon
{
public:
    std::vector<Point> vertices;
    std::vector<Point> resultadoCorte;
    std::vector<Point> areaComprimida;
    std::vector<Point> areaTracionada;
    
    void setVertices(const std::vector<Point> &points)
    {
        vertices.clear();
        for (const auto &p : points)
        {
            vertices.emplace_back(p.x, p.y);
        }
    }

    int verificarCaso(const Point &p1, const Point &p2, double cortar)
    {
        if ((p2.y <= cortar && p1.y >= cortar) || (p2.y >= cortar && p1.y <= cortar)){
            return 2; // Caso 2: Corta o trecho
        }
        if (p2.y >= cortar && p1.y >= cortar){
            return 3; // caso 3: vértices acima da linha de corte; área comprimida
        }
        if (p2.y <= cortar && p1.y <= cortar){
            return 4; // caso 4: vértices abaixo da linha de corte: área tracionada
        }
        return 1; // Caso 1: Adiciona o nó final na nova poligonal
    }

    // Função para calcular o ponto de interseção
    Point calcularIntersecao(const Point &p1, const Point &p2, double cortar)
    {
        double deltaY = p2.y - p1.y;
        double deltaX = p2.x - p1.x;

        if (std::abs(deltaX) < 1e-6)
        {
            return Point(p1.x, cortar);
        }
        else
        {
            double tg = deltaY / deltaX;
            double x = p1.x - (p1.y - cortar) / tg;
            return Point(x, cortar);
        }
    }

    void cortarPoligonal(const std::vector<Point> &vertices, float &cortar)
    {
        resultadoCorte.clear();
        areaComprimida.clear();
        areaTracionada.clear();
        int nv = vertices.size();
        std::set<Point> pontosAdicionados; // Conjunto para rastrear pontos já adicionados

        for (int i = 0; i < nv; i++)
        {
            int caso = verificarCaso(vertices[i], vertices[(i + 1) % nv], cortar);

            if (caso == 2)
            {
                Point intersecao = calcularIntersecao(vertices[i], vertices[(i + 1) % nv], cortar);
                if (pontosAdicionados.insert(intersecao).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(intersecao);
                    areaComprimida.push_back(intersecao);
                    areaTracionada.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    areaComprimida.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    areaComprimida.push_back(vertices[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    areaTracionada.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    areaTracionada.push_back(vertices[(i + 1) % nv]);
                }
            }
        }
    }
};


int main() {

    std::vector<Point> collectedPoints = {
        {0,190}, {0,178}, {50,170}, {50,45}, {25,25}, {25,0},
        {95,0}, {95, 25}, {70,45}, {70,170}, {120,178}, {120,190}
    }; 

    Polygon polygon;

    float linhaDeCorte = 100.0;

    polygon.setVertices(collectedPoints);

    polygon.cortarPoligonal(collectedPoints, linhaDeCorte);

    std::cout << "Pontos após o corte:" << std::endl;
    for (const auto& p : polygon.resultadoCorte) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Comprimida:" << std::endl;
    for (const auto& p : polygon.areaComprimida) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Tracionada:" << std::endl;
    for (const auto& p : polygon.areaTracionada) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    return 0;
}