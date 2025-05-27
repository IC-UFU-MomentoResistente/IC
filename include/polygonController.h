#pragma once

#include "createPolygonUseCase.h"
#include "getPolygonUseCase.h"
#include "savePolygonUseCase.h"
#include "polygonDto.h"
#include "polygon.h"

#include <string>
#include <vector> 
#include <optional>

class PolygonController
{
private:
    CreatePolygonUseCase createUseCase;
    GetPolygonUseCase getUseCase;
    SavePolygonUseCase saveUseCase;

public:
    PolygonController(CreatePolygonUseCase& createUI, GetPolygonUseCase& getUI, SavePolygonUseCase& saveUI);

    Polygon createNewPolygon(const PolygonDto& data);
    std::optional<Polygon> loadPolygonByName(const std::string& name);
    std::optional<Polygon> loadPolygonById(uint64_t id);
    void savePolygonExisting(Polygon& polygon);
    std::vector<std::string> getSavedPolygonNames();
};