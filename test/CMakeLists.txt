# test/CMakeLists.txt
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(test_mermaid
        ${PROJECT_SOURCE_DIR}/test/test_mermaid.c
        ${PROJECT_SOURCE_DIR}/src/utils.c
        ${PROJECT_SOURCE_DIR}/src/mermaid.c
)

set_target_properties(test_mermaid PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
