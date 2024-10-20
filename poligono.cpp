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
    std::vector<Point> translatedVertices;

    bool isClockwise() const {
        double sum = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
        }
        return sum > 0;
    }

    void ensureCounterClockwise() {
        if (isClockwise()) {
            std::reverse(vertices.begin(), vertices.end());
        }
    }

    void setVertices(const std::vector<Point> &points)
    {
        vertices.clear();
        for (const auto &p : points)
        {
            vertices.emplace_back(p.x, p.y);
        }
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

    double staticMomentX() const {
        double Sx = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            Sx += (vertices[i].y + vertices[j].y) * (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
        }
        return std::abs(Sx) / 2.0;
    }

    double staticMomentY() const {
        double Sy = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            Sy += (vertices[i].x + vertices[j].x) * (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
        }
        return std::abs(Sy) / 2.0;
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

    void rotate(double angle) {
        Point cg = centroid();
        double radians = angle * M_PI / 180.0;
        int n = translatedVertices.size();

        // Percorre cada vértice em translatedVertices usando índice
        for (size_t i = 0; i < n; ++i) {
            double x_new = (translatedVertices[i].x - cg.x) * cos(radians) - (translatedVertices[i].y - cg.y) * sin(radians) + cg.x;
            double y_new = (translatedVertices[i].x - cg.x) * sin(radians) + (translatedVertices[i].y - cg.y) * cos(radians) + cg.y;

            translatedVertices[i].x = x_new;
            translatedVertices[i].y = y_new;
        }
    }

    void getYRange(double &yMin, double &yMax) const {
        if (translatedVertices.empty()) return;

        yMin = translatedVertices[0].y;
        yMax = translatedVertices[0].y;

        int n = translatedVertices.size();

        // Percorre os vértices em translatedVertices usando índice
        for (size_t i = 1; i < n; ++i) {
            if (translatedVertices[i].y < yMin) yMin = translatedVertices[i].y;
            if (translatedVertices[i].y > yMax) yMax = translatedVertices[i].y;
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

    void cortarPoligonal(const std::vector<Point> &translatedVertices, float &cortar)
    {
        resultadoCorte.clear();
        areaComprimida.clear();
        areaTracionada.clear();
        int nv = translatedVertices.size();
        std::set<Point> pontosAdicionados; // Conjunto para rastrear pontos já adicionados

        for (int i = 0; i < nv; i++)
        {
            int caso = verificarCaso(translatedVertices[i], translatedVertices[(i + 1) % nv], cortar);

            if (caso == 2)
            {
                Point intersecao = calcularIntersecao(translatedVertices[i], translatedVertices[(i + 1) % nv], cortar);
                if (pontosAdicionados.insert(intersecao).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(intersecao);
                    areaComprimida.push_back(intersecao);
                    areaTracionada.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(translatedVertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(translatedVertices[i]);
                    areaComprimida.push_back(translatedVertices[i]);
                }
                if (pontosAdicionados.insert(translatedVertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(translatedVertices[(i + 1) % nv]);
                    areaComprimida.push_back(translatedVertices[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(translatedVertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(translatedVertices[i]);
                    areaTracionada.push_back(translatedVertices[i]);
                }
                if (pontosAdicionados.insert(translatedVertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(translatedVertices[(i + 1) % nv]);
                    areaTracionada.push_back(translatedVertices[(i + 1) % nv]);
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

    polygon.cortarPoligonal(polygon.translatedVertices, linhaDeCorte);

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