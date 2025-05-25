#pragma once

#include "polygon.h"
#include "polygonDto.h"
#include "interfacePolygonRepository.h"
#include <stdexcept>

class CreatePolygonUseCase
{
private:
    InterfacePolygonRepository& repository; // Injeção de depedência
    uint64_t nextId; // Gerador de ID

    uint64_t generateNewId();

public:
    explicit CreatePolygonUseCase(InterfacePolygonRepository& repo);

    Polygon execute(const PolygonDto& data);
};