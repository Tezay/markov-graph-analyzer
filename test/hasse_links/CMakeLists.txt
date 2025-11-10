# CMakeLists dedicated for hasse-links tests

add_executable(test_hasse_links
        test_hasse_links.c
        ${PROJECT_SOURCE_DIR}/src/list.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/hasse.c
        ${PROJECT_SOURCE_DIR}/src/tarjan.c
        ${PROJECT_SOURCE_DIR}/src/scc.c

)

set_target_properties(test_hasse_links PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
