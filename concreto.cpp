#define _USE_MATH_DEFINES
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
    std::vector<Point> areaSuperior;
    std::vector<Point> areaInferior;
    std::vector<Point> verticesTransladados;
    std::vector<Point> verticesRotacionados;
    
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

    void translacaoCG (const std::vector<Point> &points){
        verticesTransladados.clear();
        Point cg = centroide();

        for (const auto &p : points){
            float u = p.x - cg.x;
            float v = p.y - cg.y;
            verticesTransladados.emplace_back(u,v);
        }
    }

    void rotacao(double angulo) {
        verticesRotacionados.clear();
        double ang_radianos = angulo * M_PI / 180.0;

        for (const auto &p : verticesTransladados){
            float u = (p.x * cos(ang_radianos)) - (p.y * sin(ang_radianos));
            float v = (p.x * sin(ang_radianos)) + (p.y * cos(ang_radianos));
            verticesRotacionados.emplace_back(u,v);
        }
    }

    void MaxMin(float &yMin, float &yMax) const {
        if (verticesTransladados.empty()) return;

        yMin = verticesTransladados[0].y;
        yMax = verticesTransladados[0].y;

        for (const auto &p: verticesTransladados){
            if (p.y < yMin) yMin = p.y;
            if (p.y > yMax) yMax = p.y;
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
        areaSuperior.clear();
        areaInferior.clear();
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
                    areaSuperior.push_back(intersecao);
                    areaInferior.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(verticesTransladados[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[i]);
                    areaSuperior.push_back(verticesTransladados[i]);
                }
                if (pontosAdicionados.insert(verticesTransladados[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[(i + 1) % nv]);
                    areaSuperior.push_back(verticesTransladados[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(verticesTransladados[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[i]);
                    areaInferior.push_back(verticesTransladados[i]);
                }
                if (pontosAdicionados.insert(verticesTransladados[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(verticesTransladados[(i + 1) % nv]);
                    areaInferior.push_back(verticesTransladados[(i + 1) % nv]);
                }
            }
        }
    }
};

float fator_mult_tensao_comp_concreto = 0;
float epsilon_concreto_ultimo = 0;
float expoente_tensao_concreto = 0;
float epsilon_concreto_2 = 0;
float fcd;

void parametrosConcreto(float fck, float gama_c){
    if (fck <= 50){
        fator_mult_tensao_comp_concreto = 0.85;
        epsilon_concreto_ultimo = 3.5;
        expoente_tensao_concreto = 2;
        epsilon_concreto_2 = 2;
    } else {
        fator_mult_tensao_comp_concreto = 0.85 * (1 - (fck - 50) / 200);
        epsilon_concreto_ultimo = 2.6 + 35 * pow((90 - fck) / 100, 4);
        expoente_tensao_concreto = 1.4 + 23.4 * pow((90 - fck) / 100, 4);
        epsilon_concreto_2 = 2 + 0.085 * pow(fck - 50, 0.53);

        if (epsilon_concreto_2 > epsilon_concreto_ultimo) {
            epsilon_concreto_2 = epsilon_concreto_ultimo;
        }
    }
    fcd = fck / gama_c;
}

float mod_elas_aco;
float fyk;
float gama_aco;
float fyd;
float epsilon_aco;

void parametrosAco (int num_classe){
    if (num_classe == 50){
        mod_elas_aco = 210; //GPa;
        fyk = 500; //MPa;
        gama_aco = 1.15;
        fyd = fyk / gama_aco;
        epsilon_aco = mod_elas_aco * pow(10,3) / fyd;
    }
}

double Nctr = 0;
double Mctr = 0;

void EquacoesTensoesConcretoTrapezioRetangulo(float fcd, double c1, double c2, float y, float mult_fcd) {
    Nctr = mult_fcd * fcd * (c1 * y + c2 * std::pow(y, 2) / 2.0);
    Mctr = mult_fcd * fcd * (c1 * std::pow(y, 2) / 2.0 + c2 * std::pow(y, 3) / 3.0);
}

double Nctp = 0;
double Mctp = 0;

void EquacoesTensoesConcretoTrapezioParabola(float fcd, float xEpc2, float n_conc, float c1, float c2, float y, float yMax, float x_alfa, float mult_fcd){
    double g = yMax - x_alfa;
    double n1 = n_conc + 1;
    double n2 = n_conc + 2;
    double n3 = n_conc + 3;

    // Equação para eexp
    double eexp = pow((g + xEpc2 - y) / xEpc2, n1);

    // Cálculo de Nctp
    double Nctp = -mult_fcd * fcd * (
        -(xEpc2 * eexp * (c1 * n2 + c2 * (g + xEpc2 + n_conc * y + y))) / (n1 * n2)
        - c1 * y
        - (c2 * pow(y, 2)) / 2.0
    );

    // Cálculo de Mctp
    double Mctp = (mult_fcd * fcd * (
        3 * c1 * ((n1 * n2 * n3 * y * y + 2 * xEpc2 * eexp * ((yMax - x_alfa + xEpc2) * n3 + (3 + 4 * n_conc + n_conc * n_conc) * y)))
        + 2 * c2 * ((n1 * n2 * n3 * y * y * y + 3 * xEpc2 * eexp * (2 * (yMax - x_alfa) * (yMax - x_alfa) + 2 * xEpc2 * xEpc2 + 2 * xEpc2 * (n1) * y 
        + (2 + 3 * n_conc + n_conc * n_conc) * y * y + 2 * (yMax - x_alfa) * (2 * xEpc2 + (n1) * y))))
    )) / (6 * n1 * n2 * n3);

}

double Nc = 0;
double Mc = 0;

void ResultantesNormalMomentoConcreto(){
    Nc = 0;
    Mc = 0;
}

int main() {
    std::vector<Point> collectedPoints = {
        {0,190}, {0,178}, {50,170}, {50,45}, {25,25}, {25,0},
        {95,0}, {95, 25}, {70,45}, {70,170}, {120,178}, {120,190}
    }; 
    Poligono poligono;
    
    float linhaDeCorte = 0;
    float angulo = 10;
    float Ymax, Ymin;

    poligono.setVertices(collectedPoints);

    poligono.translacaoCG(poligono.vertices);

    poligono.rotacao(angulo);

    poligono.MaxMin(Ymin, Ymax);
    
    poligono.cortarPoligonal(poligono.verticesTransladados, linhaDeCorte);

    std::cout << "\nPontos Iniciais: " << std::endl;
    for (const auto& p: poligono.vertices) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }
    
    std::cout << "\nPontos transladados: " << std::endl;
    for (const auto& p: poligono.verticesTransladados) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nPontos Rotacionados:" << std::endl;
    for (const auto& p : poligono.verticesRotacionados) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nMaximos e Minimos:" << std::endl;
    std::cout << "Y max: " << Ymax << std::endl;
    std::cout << "Y min: " << Ymin << std::endl;
    
    std::cout << "\nPontos após o corte:" << std::endl;
    for (const auto& p : poligono.resultadoCorte) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Comprimida:" << std::endl;
    for (const auto& p : poligono.areaSuperior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Tracionada:" << std::endl;
    for (const auto& p : poligono.areaInferior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    float fck = 30;
    float gama_c = 1.5;

    parametrosConcreto(fck, gama_c);

    std::cout << "\nParâmetros do Concreto:" << std::endl;
    std::cout << "fator_mult_tensao_comp_concreto: " << fator_mult_tensao_comp_concreto << std::endl;
    std::cout << "epsilon_concreto_ultimo: " << epsilon_concreto_ultimo << std::endl;
    std::cout << "expoente_tensao_concreto: " << expoente_tensao_concreto << std::endl;
    std::cout << "epsilon_concreto_2: " << epsilon_concreto_2 << std::endl;
    std::cout << "fcd: " << fcd << std::endl;

    parametrosAco(50);

    std::cout << "\nParâmetros do Concreto:" << std::endl;
    std::cout << "mod_elas_aco: " << mod_elas_aco << std::endl;
    std::cout << "fyk: " << fyk << std::endl;
    std::cout << "gama_aco: " << gama_aco << std::endl;
    std::cout << "fyd: " << fyd << std::endl;
    std::cout << "epsilon_aco: " << epsilon_aco << std::endl;

    return 0;
}