# CMakeLists dedicated for matrix-ops tests (Partie 3.1)

add_executable(test_matrix_ops
        test_matrix_ops.c
        ${PROJECT_SOURCE_DIR}/src/list.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/matrix.c
)

set_target_properties(test_matrix_ops PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

