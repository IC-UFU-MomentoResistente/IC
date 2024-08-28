#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#define _USE_MATH_DEFINES

struct Point {
    double x;
    double y;

    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

class Polygon {
private:
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

public:
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

    void cutByHorizontalLine(double yCut) {
        std::vector<Point> newVertices;
        int n = vertices.size();

        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;

            if (vertices[i].y < yCut && vertices[j].y >= yCut) {
                // Interseção com a linha horizontal
                double xIntersect = vertices[i].x + (yCut - vertices[i].y) * (vertices[j].x - vertices[i].x) / (vertices[j].y - vertices[i].y);
                newVertices.emplace_back(xIntersect, yCut);
            }

            if (vertices[i].y >= yCut) {
                newVertices.emplace_back(vertices[i].x, vertices[i].y);
            }

            if (vertices[j].y >= yCut && vertices[i].y < yCut) {
                // Interseção com a linha horizontal
                double xIntersect = vertices[i].x + (yCut - vertices[i].y) * (vertices[j].x - vertices[i].x) / (vertices[j].y - vertices[i].y);
                newVertices.emplace_back(xIntersect, yCut);
            }
        }

        // Atualizar a lista de vértices
        vertices = std::move(newVertices);
        ensureCounterClockwise();
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

int main() {
    Polygon polygon;
    int numVertices;

    std::cout << "Insira o número de vértices do polígono: ";
    std::cin >> numVertices;

    for (int i = 0; i < numVertices; ++i) {
        double x, y;
        std::cout << "Insira as coordenadas do vértice " << i + 1 << " (formato: x y): ";
        std::cin >> x >> y;
        polygon.addVertex(x, y);
    }

    // Garantir que os vértices estão em ordem anti-horária
    polygon.ensureCounterClockwise();

    // Cálculo da área
    double area = polygon.area();
    std::cout << "Área: " << area << std::endl;

    // Cálculo do centro de gravidade
    Point cg = polygon.centroid();
    std::cout << "Centro de Gravidade (CG): (" << cg.x << ", " << cg.y << ")" << std::endl;

    // Translada os vértices para que o centróide coincida com a origem
    std::vector<Point> translatedVertices = polygon.translateToCG();
    std::cout << "Vértices transladados para o centróide:" << std::endl;
    for (const auto& vertex : translatedVertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
    }

    int numCuts;
    std::cout << "Digite o número de coordenadas de corte (horizontais): ";
    std::cin >> numCuts;

    if (numCuts < 1) {
        std::cerr << "O número de cortes deve ser pelo menos 1." << std::endl;
        return 1;
    }

    std::vector<double> cutCoordinates(numCuts);
    std::cout << "Digite as coordenadas de corte (uma por linha):" << std::endl;
    for (int i = 0; i < numCuts; ++i) {
        std::cout << "Corte " << i + 1 << ": ";
        std::cin >> cutCoordinates[i];
    }

    for (double cut : cutCoordinates) {
        polygon.cutByHorizontalLine(cut);
    }

    // Exibir os vértices finais após o corte
    std::cout << "Vértices após os cortes:" << std::endl;
    polygon.printVertices();

    return 0;
}
