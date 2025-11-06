# CMakeLists dedicated for class-analysis-and-export tests

add_executable(test_class_analysis_and_export
        test_class_analysis_and_export.c
        ${PROJECT_SOURCE_DIR}/src/markov_props.c
        ${PROJECT_SOURCE_DIR}/src/mermaid_hasse.c
        ${PROJECT_SOURCE_DIR}/src/utils.c
)

set_target_properties(test_class_analysis_and_export PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
