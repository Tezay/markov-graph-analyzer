# CMakeLists dedicated for io-verify tests

add_executable(test_io_verify
        test_io_verify.c
        ${PROJECT_SOURCE_DIR}/src/list.c
        ${PROJECT_SOURCE_DIR}/src/graph.c
        ${PROJECT_SOURCE_DIR}/src/io.c
        ${PROJECT_SOURCE_DIR}/src/verify.c
)

set_target_properties(test_io_verify PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
