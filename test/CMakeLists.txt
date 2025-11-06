include_directories(${PROJECT_SOURCE_DIR}/include)

# Liste des sous-répertoires de tests unitaires

# Partie 1/3
add_subdirectory(core)
add_subdirectory(io_verify)
add_subdirectory(mermaid-cli)

# Partie 2/3
add_subdirectory(tarjan_core)
add_subdirectory(class_analysis_and_export)
