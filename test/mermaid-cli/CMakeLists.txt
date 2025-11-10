# CMakeLists dedicated for mermaid-cli tests

add_executable(test_mermaid-cli
        test_mermaid.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/list.c
        ${PROJECT_SOURCE_DIR}/src/utils.c
        ${PROJECT_SOURCE_DIR}/src/mermaid.c
)

set_target_properties(test_mermaid_cli PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
