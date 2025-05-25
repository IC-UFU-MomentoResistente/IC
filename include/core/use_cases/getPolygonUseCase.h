#pragma once

#include "polygon.h"
#include "interfacePolygonRepository.h"
#include <optional>
#include <vector>
#include <stdexcept>

class GetPolygonUseCase
{
private:
    InterfacePolygonRepository& repository;

public:
    explicit GetPolygonUseCase(InterfacePolygonRepository& repo);

    std::optional<Polygon> executeByName(const std::string& name);
    std::optional<Polygon> executeById(uint64_t id);
    std::vector<std::string> listSavedSectionNames();
};