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

class Poligono{
public:
    std::vector<Point> vertices;
    std::vector<Point> resultadoCorte;
    std::vector<Point> areaComprimida;
    std::vector<Point> areaTracionada;
    std::vector<Point> verticesTransladados;

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

    Point centroide() const {
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
        Point cg = centroide();
        std::vector<Point> verticesTransladados;
        int n = vertices.size();

            for (size_t i = 0; i < n; ++i) {
                const Point& vertex = vertices[i];
                
                double translatedX = vertex.x - cg.x;
                double translatedY = vertex.y - cg.y;

                verticesTransladados.emplace_back(translatedX, translatedY);
            }

        return verticesTransladados;
    }

    void rotate(double angle) {
        Point cg = centroide();
        double radians = angle * M_PI / 180.0;
        int n = verticesTransladados.size();

        // Percorre cada vértice em verticesTransladados usando índice
        for (size_t i = 0; i < n; ++i) {
            double x_new = (verticesTransladados[i].x - cg.x) * cos(radians) - (verticesTransladados[i].y - cg.y) * sin(radians) + cg.x;
            double y_new = (verticesTransladados[i].x - cg.x) * sin(radians) + (verticesTransladados[i].y - cg.y) * cos(radians) + cg.y;

            verticesTransladados[i].x = x_new;
            verticesTransladados[i].y = y_new;
        }
    }

    void getYRange(double &yMin, double &yMax) const {
        if (verticesTransladados.empty()) return;

        yMin = verticesTransladados[0].y;
        yMax = verticesTransladados[0].y;

        int n = verticesTransladados.size();

        // Percorre os vértices em verticesTransladados usando índice
        for (size_t i = 1; i < n; ++i) {
            if (verticesTransladados[i].y < yMin) yMin = verticesTransladados[i].y;
            if (verticesTransladados[i].y > yMax) yMax = verticesTransladados[i].y;
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

    void cortarPoligonal(const std::vector<Point> &verticesTransladados, float &cortar)
    {
        resultadoCorte.clear();
        areaComprimida.clear();
        areaTracionada.clear();
        int nv = verticesTransladados.size();
        std::set<Point> pontosAdicionados; // Conjunto para rastrear pontos já adicionados

        for (int i = 0; i < nv; i++)
        {
            int caso = verificarCaso(verticesTransladados[i], verticesTransladados[(i + 1) % nv], cortar);

            if (caso == 2)
            {
                Point intersecao = calcularIntersecao(verticesTransladados[i], verticesTransladados[(i + 1) % nv], cortar);
                if (pontosAdicionados.insert(intersecao).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(intersecao);
                    areaComprimida.push_back(intersecao);
                    areaTracionada.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(verticesTransladados[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[i]);
                    areaComprimida.push_back(verticesTransladados[i]);
                }
                if (pontosAdicionados.insert(verticesTransladados[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[(i + 1) % nv]);
                    areaComprimida.push_back(verticesTransladados[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(verticesTransladados[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[i]);
                    areaTracionada.push_back(verticesTransladados[i]);
                }
                if (pontosAdicionados.insert(verticesTransladados[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[(i + 1) % nv]);
                    areaTracionada.push_back(verticesTransladados[(i + 1) % nv]);
                }
            }
        }
    }
};

class Concreto{
    public:
    Poligono poligono;  
    std::vector<Point> verticesComprimidos;
    double fck, alfa_c, lambda, Epcu, n_conc, Epc2, fcd;

    Concreto(const std::vector<Point>& vertices) {
        poligono.setVertices(vertices);
    }

    void calcularAreaComprimida(float linhaDeCorte) {
        poligono.verticesTransladados = poligono.translateToCG();
        poligono.cortarPoligonal(poligono.verticesTransladados, linhaDeCorte);
        verticesComprimidos = poligono.areaComprimida;
    }

    void definirParametrosConcreto(double fck) {
        if (fck <= 50) {
            alfa_c = 0.85;
            lambda = 0.8;
            Epcu = 3.5;  // Por mil Em milímetros
            n_conc = 2;
            Epc2 = 2;    // Por mil Em milímetros
        } else {
            alfa_c = 0.85 * (1 - (fck - 50) / 200);
            lambda = 0.8 - (fck - 50) / 400;
            Epcu = 2.6 + 35 * pow((90 - fck) / 100, 4);
            n_conc = 1.4 + 23.4 * pow((90 - fck) / 100, 4);
            Epc2 = 2 + 0.085 * pow(fck - 50, 0.53);
            if (Epc2 > Epcu) {
                Epc2 = Epcu;
            }
        }
        fcd = fck / 1.4;  // Exemplo: gama_c definido como 1.4
    }

    std::pair<double,double> Nctr_Mctr(double fcd, double c1, double c2, double y, double mult_fcd) {
        double Nctr = mult_fcd * fcd * (c1 * y + c2 * y * y / 2.0);
        double Mctr = mult_fcd * fcd * (c1 * y * y / 2.0 + c2 * y * y * y / 3.0);
        return {Nctr, Mctr};
    }

    std::pair<double,double> Nctp_Mctp(double fcd, double xEpc2, double n_conc, double c1, double c2, double y, double ymax, double xalfa, double mult_fcd) {
        
    }

    std::pair<double, double> calcularNcMc(double EpA, double xalfa, double Epap, double d, const std::vector<Point>& verticesComprimidos, double vmax, double mult_fcd) {
        double Nc = 0, Mc = 0;
        double k = (Epap - EpA) / d / 1000.0;

        

        return { Nc * 1E3, Mc * 1E3 }; // Retorna em kN e kN*m
    }
};
    

class Aco{
    public:
    
    void definirParametrosAco{
        double fyk, fyd, Es, gama_s, Eps, Epyd;
        fyk = 500; //MPa
        gama_s = 1.15;
        fyd = fyk/gama_s;
        Es = 210; //GPa;
    }

    std::pair<double, double> calcularNpMp;

};


int main() {

    std::vector<Point> collectedPoints = {
        {0,190}, {0,178}, {50,170}, {50,45}, {25,25}, {25,0},
        {95,0}, {95, 25}, {70,45}, {70,170}, {120,178}, {120,190}
    }; 

    Poligono poligono;

    float linhaDeCorte = 100.0;

    poligono.setVertices(collectedPoints);

    poligono.cortarPoligonal(poligono.verticesTransladados, linhaDeCorte);

    std::cout << "Pontos após o corte:" << std::endl;
    for (const auto& p : poligono.resultadoCorte) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Comprimida:" << std::endl;
    for (const auto& p : poligono.areaComprimida) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Tracionada:" << std::endl;
    for (const auto& p : poligono.areaTracionada) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    return 0;
}