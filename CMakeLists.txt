cmake_minimum_required(VERSION 3.16)
project(markov-graph-analyzer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options de compilation (ajuste -Werror selon le niveau attendu)
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dossiers d'include
include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources
set(SRC
        src/main.c
        src/list.c
        src/graph.c
        src/io.c
        src/matrix.c
        src/mermaid.c
        src/utils.c
        src/verify.c
        src/scc.c
        src/tarjan.c
        src/hasse.c
        src/markov_props.c
        src/mermaid_hasse.c
        src/period.c
)

add_executable(markov_graph_analyzer ${SRC})

# Chemins indépendants du répertoire courant
add_compile_definitions(
        DATA_DIR="${CMAKE_SOURCE_DIR}/data"
        OUT_DIR="${CMAKE_SOURCE_DIR}/out/mermaid"
)

add_subdirectory(test)
