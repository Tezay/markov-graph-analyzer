# Tests unitaires — markov-graph-analyzer

Ce dossier contient des exécutables de test simples (sans CTest/GTest) pour vérifier chaque sous-partie du projet (jusqu'à **Partie 2.1**)

## Prérequis
- CMake ≥ 3.16 et un compilateur C (GCC/Clang/MSVC)
- CLion (optionnel) si vous voulez exécuter via l’IDE

## Arborescence des tests
### Partie 1
- **Etape 1 :** `test/core` → cible `test_core` (structures de base `graph`/`list`)
- **Etape 2 :** `test/io_verify` → cible `test_io_verify` (lecture de fichiers + vérification Markov)
- **Etape 3 :** `test/mermaid_cli` → cible `test_mermaid_cli` (utils + export Mermaid et vérifications basiques)
### Partie 2
- **Etape 1 :** `test/tarjan_core` → cible `test_tarjan_core` (SCC via Tarjan, partition)

Chaque sous-dossier possède son propre `CMakeLists.txt` qui déclare un exécutable `test_*` et fixe:
- `RUNTIME_OUTPUT_DIRECTORY` = dossier de build (pour retrouver facilement les binaires)
- `WORKING_DIRECTORY` = racine du projet (pour que les chemins `data/` et `out/` fonctionnent sans configuration supplémentaire)

## Exécuter via CLion
1) Ouvrez la racine du projet dans CLion et laissez CMake s’indexer.
2) Les cibles `test_core`, `test_io_verify`, `test_mermaid_cli`, `test_tarjan_core` apparaissent dans la liste des configurations.
3) Sélectionnez la cible souhaitée et lancez-la (Run ▶). Le répertoire de travail est défini à la racine du projet par CMake; si besoin, ajustez-le dans Run | Edit Configurations.

## Détails par test

### core (`test/core/test_core.c`)
- But: valider l’initialisation du graphe, l’ajout d’arêtes et l’affichage.
- Démarche: crée un petit graphe en mémoire, appelle `graph_print`, puis libère.
- Sortie attendue: une liste d’adjacence cohérente, sans crash ni fuite apparente.

### io_verify (`test/io_verify/test_io_verify.c`)
- But: valider la lecture des graphes texte et la vérification Markov.
- Démarche: parcourt `DATA_DIR` et teste tous les `*.txt`:
  - lit avec `read_graph_from_file`
  - compte les arêtes
  - vérifie `verify_markov(g, 0.01)`
- Résultat: affiche un récapitulatif `[SUMMARY] files=X ok=Y ko=Z` et retourne `Z` (nb d’échecs de vérification).

### mermaid_cli (`test/mermaid_cli/test_mermaid.c`)
- But: tester des utilitaires (`get_id_alpha`, `near_one`, `ensure_dir`) et l’export Mermaid.
- Démarche:
  - construit deux graphes en mémoire (valide et volontairement invalide pour Markov)
  - exporte en `.mmd` sous `out/`
  - écrit un petit fichier `data/exemple1.txt`, le relit puis exporte
  - utilise des « mocks » locaux de `verify_markov`/`read_graph_from_file` pour isoler la partie Mermaid/Utils
- Résultat: messages de vérification et présence de fichiers `.mmd` non vides.

### tarjan_core (`test/tarjan_core/test_tarjan_core.c`)
- But: valider l’algorithme de Tarjan et la construction de la partition en CFC.
- Démarche: lit `data/exemple_valid_step3.txt`, exécute `tarjan_partition`, affiche les classes et construit un mapping sommet→classe pour vérification simple.
- Résultat: affichage d’une partition plausible, fin de programme avec `[OK] Tarjan partition script executed.`

## À propos des CMakeLists locaux
- `test/CMakeLists.txt` ajoute chaque sous-répertoire et déclare un exécutable par test.
- Chaque `CMakeLists.txt` de sous-dossier liste explicitement les sources du projet nécessaires (ex.: `src/graph.c`, `src/tarjan.c`, etc.).
- Le projet racine définit `DATA_DIR` et `OUT_DIR` avant `add_subdirectory(test)`, ce qui rend ces macros visibles dans les sous-dossiers. Si vous déplacez `data/` ou `out/`, modifiez-les dans `CMakeLists.txt` à la racine.

