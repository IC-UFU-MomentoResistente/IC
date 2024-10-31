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
    std::vector<Point> AreaSuperior;
    std::vector<Point> AreaInferior;
    
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
        AreaSuperior.clear();
        AreaInferior.clear();
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
                    AreaSuperior.push_back(intersecao);
                    AreaInferior.push_back(intersecao);
                }
            }
            if (caso == 3)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    AreaSuperior.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    AreaSuperior.push_back(vertices[(i + 1) % nv]);
                }
            }
            if (caso == 4)
            {
                if (pontosAdicionados.insert(vertices[i]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[i]);
                    AreaInferior.push_back(vertices[i]);
                }
                if (pontosAdicionados.insert(vertices[(i + 1) % nv]).second) // Adiciona se não existir
                {
                    resultadoCorte.push_back(vertices[(i + 1) % nv]);
                    AreaInferior.push_back(vertices[(i + 1) % nv]);
                }
            }
        }
    }

    void fecharPoligono(std::vector<Point>& pontos) {
        if (!pontos.empty()) {
            pontos.push_back(pontos[0]);
        }
    }
};

float coef_mult_tensão_concreto = 0;
float epsilon_concreto_ultimo = 0;
float expoente_tensão_concreto = 0;
float epsilon_concreto_2 = 0;
float fcd;

void parametrosConcreto(float fck, float gama_c){
    if (fck <= 50){
        coef_mult_tensão_concreto = 0.85;
        epsilon_concreto_ultimo = 3.5;
        expoente_tensão_concreto = 2;
        epsilon_concreto_2 = 2;
    } else {
        coef_mult_tensão_concreto = 0.85 * (1 - (fck - 50) / 200);
        epsilon_concreto_ultimo = 2.6 + 35 * pow((90 - fck) / 100, 4);
        expoente_tensão_concreto = 1.4 + 23.4 * pow((90 - fck) / 100, 4);
        epsilon_concreto_2 = 2 + 0.085 * pow(fck - 50, 0.53);

        if (epsilon_concreto_2 > epsilon_concreto_ultimo) {
            epsilon_concreto_2 = epsilon_concreto_ultimo;
        }
    }
    fcd = fck / gama_c;
}

int main() {
        // Variáveis globais
    std::vector<Point> collectedPoints = {
        {0,190}, {0,178}, {50,170}, {50,45}, {25,25}, {25,0},
        {95,0}, {95, 25}, {70,45}, {70,170}, {120,178}, {120,190}
    }; // Armazenar os pontos coletados

    Poligono poligono;
    float linhaDeCorte = 100.0;

    poligono.setVertices(collectedPoints);

    poligono.cortarPoligonal(poligono.vertices, linhaDeCorte);

    std::cout << "Pontos após o corte:" << std::endl;
    for (const auto& p : poligono.resultadoCorte) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Comprimida:" << std::endl;
    for (const auto& p : poligono.AreaSuperior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    std::cout << "\nÁrea Tracionada:" << std::endl;
    for (const auto& p : poligono.AreaInferior) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }

    
    float fck = 30;
    float gama_c = 1.5;

    // Chamar a função para modificar as variáveis globais
    parametrosConcreto(fck, gama_c);

    // Acessar e exibir os valores modificados
    std::cout << "Parâmetros do Concreto:" << std::endl;
    std::cout << "coef_mult_tensão_concreto: " << coef_mult_tensão_concreto << std::endl;
    std::cout << "epsilon_concreto_ultimo: " << epsilon_concreto_ultimo << std::endl;
    std::cout << "expoente_tensão_concreto: " << expoente_tensão_concreto << std::endl;
    std::cout << "epsilon_concreto_2: " << epsilon_concreto_2 << std::endl;
    std::cout << "fcd: " << fcd << std::endl;

    return 0;
}