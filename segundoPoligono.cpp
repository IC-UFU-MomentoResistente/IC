#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

struct Point {
    double x;
    double y;

    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

class Polygon{
public:

    std::vector<Point> vertices;

bool isClockwise() const {
    double sum = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
    }
    return sum > 0;
}

void addVertex(double x, double y) {
    vertices.emplace_back(x, y);
}

void ensureCounterClockwise() {
    std::cout << "Vértices antes de garantir o sentido anti-horário:" << std::endl;
    printVertices(); // Exibe os vértices antes da alteração

    if (isClockwise()) {
        std::reverse(vertices.begin(), vertices.end());
    }

    std::cout << "Vértices depois de garantir o sentido anti-horário:" << std::endl;
    printVertices(); // Exibe os vértices após a alteração
}

double area() const {
    double A = 0.0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        A += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
    }
    return std::abs(A) / 2.0;
}

Point centroid() const {
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

std::vector<Point> translateToCG() const {
    Point cg = centroid();
    std::vector<Point> translatedVertices;
    int n = vertices.size();

    for (size_t i = 0; i < n; ++i) {
        const Point& vertex = vertices[i];
        double translatedX = vertex.x - cg.x;
        double translatedY = vertex.y - cg.y;
        translatedVertices.emplace_back(translatedX, translatedY);
    }

    return translatedVertices;
}

void intersecao_linha_neutra(double y_neutra, double epsilon) {
    std::vector<Point> nova_lista_vertices;
    int n = vertices.size();

    for (int i = 0; i < n; ++i) {
        Point vertice_atual = vertices[i];
        Point proximo_vertice = vertices[(i + 1) % n];
        
        double xi = vertice_atual.x, yi = vertice_atual.y;
        double xj = proximo_vertice.x, yj = proximo_vertice.y;
        
        // Adicionar o vértice atual à nova lista se não está sobre a linha neutra
        
        // verificar a condição, provavelmente yi - y_neutra > 0
        if (fabs(yi - y_neutra) >= epsilon) {
            nova_lista_vertices.push_back(vertice_atual);
        }
        
        // Verificar aresta horizontal na linha neutra
        // Verificar pq não entra no continue
        if (fabs(yi - y_neutra) < epsilon && fabs(yj - y_neutra) < epsilon) {
            continue; // Aresta é paralela à linha neutra, ignora interseção
        }
        
        // Verificar interseção real
        if ((yi - y_neutra) * (yj - y_neutra) < 0) {
            double t = (y_neutra - yi) / (yj - yi);
            double x_intersecao = xi + t * (xj - xi);
            double y_intersecao = y_neutra;
            
            // Evitar duplicação de pontos e manter a ordem dos vértices
            // Evitar a lógica de ir alimentando a nova lista 
            if (nova_lista_vertices.empty() || 
                (fabs(nova_lista_vertices.back().x - x_intersecao) > epsilon || 
                 fabs(nova_lista_vertices.back().y - y_intersecao) > epsilon)) {
                nova_lista_vertices.emplace_back(x_intersecao, y_intersecao);
            }
        }
    }

    vertices = std::move(nova_lista_vertices);
    ensureCounterClockwise(); // Garantir que os vértices estão no sentido anti-horário
}

void printVertices() const {
    for (const auto& vertex : vertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
    }
}

std::vector<Point> getVertices() const {
    return vertices;
}

};


