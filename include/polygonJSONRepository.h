#pragma once

#include "interfacePolygonRepository.h"
#include "polygon.h"
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

namespace fs = std::filesystem;

class PolygonJSONRepository : public InterfacePolygonRepository
{
private:
    fs::path dataDirectory; // onde os arquivos JSON serão armazenados
    uint64_t nextId; // simular auto-incremento de ID

    std::string generateNameArchive(const std::string& name) const;
    std::string generateNameArchive(uint64_t id) const;
    void loadNextId();

public:
    explicit PolygonJSONRepository(const std::string dataPathDirectory);

    void save(Polygon& polygon) override;
    std::optional<Polygon> loadByName(const std::string& name) override;
    std::optional<Polygon> loadById(uint64_t id) override;
    std::vector<std::string> listSavedSectionNames() override;
};