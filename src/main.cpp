#include <iostream>
#include <string>
#include <limits>
#include <sstream>

#include "polygon.h"

int main() {
    Polygon polygon;
    int numVertices;

    // Solicita o número de vértices
    std::cout << "Digite o número de vértices do polígono:" << std::endl;
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        std::istringstream iss(input);
        if (iss >> numVertices && numVertices > 2) {
            break;  // Sai do loop se o número de vértices for válido (maior que 2)
        } else {
            std::cout << "Número inválido. Por favor, insira um número maior que 2:" << std::endl;
        }
    }

    // Solicita os vértices
    std::cout << "Digite os " << numVertices << " vértices do polígono (x y):" << std::endl;
    for (int i = 0; i < numVertices; ++i) {
        double x, y;
        while (true) {
            std::string input;
            std::getline(std::cin, input);
            std::istringstream iss(input);
            if (iss >> x >> y) {
                polygon.addVertex(x, y);
                break;
            } else {
                std::cout << "Entrada inválida. Digite um número (x y):" << std::endl;
            }
        }
    }

    polygon.ensureCounterClockwise();
    
    std::cout << "Área do polígono: " << polygon.area() << std::endl;

    Point centroid = polygon.centroid();
    std::cout << "Centro de gravidade do polígono: (" << centroid.x << ", " << centroid.y << ")" << std::endl;

    std::cout << "Digite a coordenada y da linha neutra para o corte:" << std::endl;
    double y_neutra;
    std::cin >> y_neutra;

    polygon.intersecao_linha_neutra(y_neutra);

    std::cout << "Vértices após o corte com a linha neutra:" << std::endl;
    polygon.printVertices();

    return 0;
}
