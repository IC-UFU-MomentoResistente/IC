#pragma once

#include "polygon.h"
#include <optional>
#include <vector>
#include <string>

class InterfacePolygonRepository
{
public:
    virtual ~InterfacePolygonRepository() = default;

    virtual void save(Polygon& polygon) = 0;
    virtual std::optional<Polygon> loadByName(const std::string& name) = 0;
    virtual std::optional<Polygon> loadById(uint64_t id) = 0;
    virtual std::vector<std::string> listSavedSectionNames() = 0;
};