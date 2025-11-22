add_executable(test_stationary_analysis
        test_stationary_analysis.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/scc.c
        ${PROJECT_SOURCE_DIR}/src/matrix.c
        ${PROJECT_SOURCE_DIR}/src/list.c
)

set_target_properties(test_stationary_analysis PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)