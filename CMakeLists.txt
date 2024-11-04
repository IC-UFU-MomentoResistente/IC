cmake_minimum_required(VERSION 3.10)
project(teste)

# Versão do C++
set(CMAKE_CXX_STANDARD 11)

# Incluir diretórios de cabeçalhos
include_directories(include)
include_directories(lib/raylib/src)
include_directories(lib/imGui)
include_directories(lib/implot)
include_directories(lib/rlImGui)

# Arquivos fonte
set(SOURCES
    src/main.cpp
    src/poligono.cpp
)

# Adicionar executável
add_executable(MyProject ${SOURCES})

# Linkar bibliotecas (substitua por caminho para as bibliotecas, se necessário)
target_link_libraries(MyProject
    ${CMAKE_SOURCE_DIR}/lib/raylib/raylib.lib
    ${CMAKE_SOURCE_DIR}/lib/imGui/imgui.lib
    ${CMAKE_SOURCE_DIR}/lib/implot/implot.lib
    ${CMAKE_SOURCE_DIR}/lib/rlImGui/rlImGui.lib
    winmm.lib
)

# Configuração para Linux: necessário para o Raylib
if (UNIX AND NOT APPLE)
    target_link_libraries(teste m pthread dl)
endif()
