#include "polygonController.h"

PolygonController::PolygonController(CreatePolygonUseCase& createUI, GetPolygonUseCase& getUI, SavePolygonUseCase& saveUI)
    : createUseCase(createUI), getUseCase(getUI), saveUseCase(saveUI) {}

// chamada pela UI para criar um nova seção
// retorna a seção criada ou lança uma exeção em caso de erro
// ideal a UI coletar os dados (nome, vertices) e passar como DTO
Polygon PolygonController::createNewPolygon(const PolygonDto& data) { return createUseCase.execute(data); }

// chamada pela UI para carregar uma seção
std::optional<Polygon> PolygonController::loadPolygonByName(const std::string& name) { return getUseCase.executeByName(name); }

std::optional<Polygon> PolygonController::loadPolygonById(uint64_t id) { return getUseCase.executeById(id); }

// chamada pela UI para salvar uma seção existente 
void PolygonController::savePolygonExisting(Polygon& polygon) { saveUseCase.execute(polygon); }

std::vector<std::string> PolygonController::getSavedPolygonNames() { return getUseCase.listSavedSectionNames(); }