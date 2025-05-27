#include "savePolygonUseCase.h" 

SavePolygonUseCase::SavePolygonUseCase(InterfacePolygonRepository& repo)
    : repository(repo) {}

void SavePolygonUseCase::execute(Polygon& polygon)
{
    if (polygon.name.empty())
    {
        throw std::invalid_argument("O nome da seção não pode estar vazio.");
    }
    if (!polygon.isValid())
    {
        throw std::runtime_error("Os vértices não formam uma poligonal válida");
    }

    repository.save(polygon);
}