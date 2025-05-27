#include "getPolygonUseCase.h"

GetPolygonUseCase::GetPolygonUseCase(InterfacePolygonRepository& repo)
    : repository(repo) {}

std::optional<Polygon> GetPolygonUseCase::executeByName(const std::string& name) { return repository.loadByName(name); }

std::optional<Polygon> GetPolygonUseCase::executeById(uint64_t id) { return repository.loadById(id); }

std::vector<std::string> GetPolygonUseCase::listSavedSectionNames() { return repository.listSavedSectionNames(); }