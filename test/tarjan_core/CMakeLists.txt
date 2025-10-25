# CMakeLists dedicated for tarjan-core tests

add_executable(test_tarjan_core
        test_tarjan_core.c
        ${PROJECT_SOURCE_DIR}/src/list.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/scc.c
        ${PROJECT_SOURCE_DIR}/src/tarjan.c
        ${PROJECT_SOURCE_DIR}/src/io.c
)

set_target_properties(test_tarjan_core PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
