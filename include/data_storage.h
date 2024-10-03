#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

// Definindo a estrutura Ponto
struct Ponto {
    float x, y;
};

void InitData();
void SetNumPoints(int numPointsInput);
void GetTableData(int index, float *x, float *y);
void SetTableData(int index, float x, float y);
int GetNumPoints();
void FreeData(); // Adiciona a declaração da função

#endif // DATA_STORAGE_H
