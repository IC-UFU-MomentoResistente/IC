/* #define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

struct Point {
    double x;
    double y;

    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

/* 

class Polygon {
private:
    std::vector<Point> vertices;

  /*  bool isClockwise() const {
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
    

    void updateVerticesWithTranslated() {
        std::vector<Point> translatedVertices = translateToCG();
        vertices = std::move(translatedVertices);
    }
    

    void SP_Corte(int i, int j, double VLN, std::vector<Point>& newVertices) {
        double DELTAW = vertices[j].y - vertices[i].y;
        double DELTAU = vertices[j].x - vertices[i].x;

        if (std::abs(DELTAU) < 0.01) {
            return;
        }

        double COEFANG = DELTAW / DELTAU;
        double xIntersect = vertices[i].x - (vertices[i].y - VLN) / COEFANG;
        newVertices.emplace_back(xIntersect, VLN);
    }

    std::vector<Point> cutByHorizontalLine(double VLN) {
        std::vector<Point> newVertices;
        int NV = vertices.size();

        for (int i = 0; i < NV; i++) {
            int j = (i + 1) % NV;


            if(vertices[i].y <= VLN && vertices[j].y >= VLN){
                SP_Corte(i, j, VLN, newVertices);
            }

            if (vertices[i].y >= VLN && vertices[j].y <= VLN) {
                SP_Corte(i, j, VLN, newVertices);
            }

            if (vertices[i].y >= VLN && vertices[j].y >= VLN) {
                newVertices.emplace_back(vertices[i].x, vertices[i].y);
            }
        }

        return newVertices;
        // ensureCounterClockwise();
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


*/


/* int main() {
    // Criação de um polígono
    Polygon polygon;

    // Adicionando vértices ao polígono
    polygon.addVertex(0.0, 0.0);
    polygon.addVertex(4.0, 0.0);
    polygon.addVertex(4.0, 4.0);
    polygon.addVertex(0.0, 4.0);

    // Exibindo os vértices do polígono original
    std::cout << "Polígono original:" << std::endl;
    polygon.printVertices();

    // Garantindo que os vértices estejam no sentido anti-horário
    polygon.ensureCounterClockwise();

    // Calculando a área do polígono
    double polygonArea = polygon.area();
    std::cout << "Área do polígono: " << polygonArea << std::endl;

    // Calculando o centróide do polígono
    Point centroid = polygon.centroid();
    std::cout << "Centróide do polígono: (" << centroid.x << ", " << centroid.y << ")" << std::endl;

    // Transladando o polígono para que o centróide fique na origem
    std::vector<Point> translatedVertices = polygon.translateToCG();
    std::cout << "Polígono transladado (centróide na origem):" << std::endl;
    for (const auto& vertex : translatedVertices) {
        std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
    }

    // Atualizando os vértices com os transladados
    polygon.updateVerticesWithTranslated();

    // Cortando o polígono por uma linha horizontal (VLN = 2.0)
    double VLN = 2.0;
    std::cout << "Cortando o polígono na linha horizontal VLN = " << VLN << std::endl;
    polygon.cutByHorizontalLine(VLN);

    // Exibindo os vértices após o corte
    std::cout << "Polígono após o corte:" << std::endl;
    polygon.printVertices();

    return 0;
} */