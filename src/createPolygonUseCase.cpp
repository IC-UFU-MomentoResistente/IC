#include "createPolygonUseCase.h"

uint64_t CreatePolygonUseCase::generateNewId() {return nextId++; }

CreatePolygonUseCase::CreatePolygonUseCase(InterfacePolygonRepository& repo)
    : repository(repo), nextId(1) {}

Polygon CreatePolygonUseCase::execute(const PolygonDto& data)
{
    if (data.name.empty())
    {
        throw std::invalid_argument("O nome da seção não pode estar vazio.");
    }
    if (data.vertices.size() < 3)
    {
        throw std::invalid_argument("Uma seção trasnversal requer pelo menos 3 pontos");
    }

    uint64_t newId = generateNewId();
    Polygon newPolygon(newId, data.name, data.vertices);

    if(!newPolygon.isValid())
    {
        throw std::runtime_error("Os vértices não formam uma poligonal válida");
    }

    repository.save(newPolygon);
    return newPolygon;
}