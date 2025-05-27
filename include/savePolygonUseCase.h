#pragma once

#include "polygon.h"
#include "interfacePolygonRepository.h"
#include <stdexcept>

class SavePolygonUseCase
{
private:
    InterfacePolygonRepository& repository; // Injeção de dependência

public: 
    explicit SavePolygonUseCase(InterfacePolygonRepository& repo);

    void execute(Polygon& polygon);
};