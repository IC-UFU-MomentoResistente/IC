#include "data_storage.h"
#include <vector>  // Para usar std::vector

static std::vector<Ponto> pointsData; // Usando std::vector para gerenciar pontos

void InitData() {
    pointsData.clear(); // Inicializa a memória para os dados dos pontos
}

void SetNumPoints(int numPointsInput) {
    if (numPointsInput < 0) numPointsInput = 0;
    pointsData.resize(numPointsInput); // Ajusta o tamanho do vetor
}

void GetTableData(int index, float* x, float* y) {
    if (index < 0 || index >= pointsData.size()) return; // Verifica limites
    *x = pointsData[index].x;
    *y = pointsData[index].y;
}

void SetTableData(int index, float x, float y) {
    if (index < 0 || index >= pointsData.size()) return; // Verifica limites
    pointsData[index].x = x;
    pointsData[index].y = y;
}

int GetNumPoints() {
    return pointsData.size(); // Retorna o tamanho do vetor
}

void FreeData() {
    pointsData.clear(); // Limpa os dados
}
