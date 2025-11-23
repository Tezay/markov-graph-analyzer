# Tests unitaires — markov-graph-analyzer

Ce dossier contient des exécutables de test simples (sans CTest/GTest) pour vérifier chaque sous-partie du projet (Parties 1 à 3 + défi période)

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
- **Etape 2 :** `test/hasse_links` → cible `test_hasse_links` (liens inter-classes/Hasse)
- **Etape 3 :** `test/class_analysis_and_export` → cible `test_class_analysis_and_export` (typage des classes, irréductibilité, absorbants, exports)
### Partie 3
- **Etape 1 :** `test/matrix_ops` → cible `test_matrix_ops` (matrices de transition, puissances et distributions)
- **Etape 2 :** `test/stationary_analysis` → cible `test_stationary_analysis` (sous-matrices par classe et distributions stationnaires)
- **Défi période :** `test/period_analysis` → cible `test_period` (période des classes et unicité stationnaire)

Chaque sous-dossier possède son propre `CMakeLists.txt` qui déclare un exécutable `test_*` et fixe:
- `RUNTIME_OUTPUT_DIRECTORY` = dossier de build (pour retrouver facilement les binaires)
- `WORKING_DIRECTORY` = racine du projet (pour que les chemins `data/` et `out/` fonctionnent sans configuration supplémentaire)

## Exécuter via CLion
1) Ouvrez la racine du projet dans CLion et laissez CMake s’indexer.
2) Les cibles `test_core`, `test_io_verify`, `test_mermaid_cli`, `test_tarjan_core`, `test_hasse_links`, `test_class_analysis_and_export`, `test_matrix_ops`, `test_stationary_analysis`, `test_period` apparaissent dans la liste des configurations.
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
- Résultat: affiche un récapitulatif `[RÉSUMÉ] fichiers=X ok=Y ko=Z` et retourne `Z` (nb d’échecs de vérification).

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
- Résultat: affichage d’une partition plausible, fin de programme avec `[OK] Partition Tarjan exécutée.`

### hasse_links (`test/hasse_links/test_hasse_links.c`)
- But: construire et afficher les liens inter-classes (Hasse) à partir d’un graphe et de sa partition en CFC.
- Démarche:
  - construit un graphe d’exemple en mémoire
  - calcule la partition via `tarjan_partition`
  - appelle `build_class_links` pour produire les liens entre classes
  - affiche la partition et les liens
- Résultat: liste claire des classes et des arcs « Classe i -> Classe j » (aucun crash; liens cohérents avec le graphe).

### class_analysis_and_export (`test/class_analysis_and_export/test_class_analysis_and_export.c`)
- But: analyser les classes (transientes/persistantes, irréductibilité, états absorbants) et tester les exports Mermaid/texte.
- Démarche:
  - construit une partition de test déterministe et des liens Hasse
  - appelle `markov_class_types`, `markov_is_irreducible`, `markov_is_absorbing_vertex`
  - crée `out/` si besoin puis exporte avec `export_hasse_mermaid` et `export_partition_text`
- Résultat: affichage des drapeaux transiente/persistante, du statut d’irréductibilité, du caractère absorbant de quelques sommets; création de `out/hasse_test.mmd` et `out/partition_test.txt` non vides.

### matrix_ops (`test/matrix_ops/test_matrix_ops.c`)
- But: implémenter et valider les opérations matricielles (Partie 3.1) et les distributions \(\Pi_t = \Pi_0 M^t\).
- Démarche:
  - construit un petit graphe de météo en mémoire (Cloudy, Rainy, Sunny),
  - génère la matrice de transition `M` via `mx_from_adjlist`,
  - affiche `M`, `M^3` et `M^7` avec `mx_print`,
  - calcule des distributions `Pi_t` en partant de Cloudy puis de Rainy avec `dist_power`,
  - illustre un test de convergence via `mx_diff_abs1(M^k, M^{k+1}) < eps`.
- Résultat: affichage lisible des matrices et distributions, valeurs numériques raisonnables (probabilités positives et sommes proches de 1), aucune erreur ni fuite apparente.

### stationary_analysis (`test/stationary_analysis/test_stationary_analysis.c`)
- But: vérifier l’extraction des sous-matrices par classe (Partie 3.2) et la cohérence des contenus.
- Démarche: construit des partitions déterministes, extrait des sous-matrices, compare les valeurs attendues.
- Résultat: sous-matrices correctes (tailles et coefficients), tests verts si l’extraction est valide.

### period_analysis (`test/period_analysis/test_period.c`)
- But: calculer la période d’une classe (défi Partie 3.3) et l’unicité de la stationnaire (période = 1).
- Démarche: matrices simples (cycles de période 3, 1, 2), calcule `class_period` et `class_has_unique_stationary`.
- Résultat: périodes détectées (3, 1, 2) et drapeau “stationnaire unique” cohérent.

## À propos des CMakeLists locaux
- `test/CMakeLists.txt` ajoute chaque sous-répertoire et déclare un exécutable par test.
- Chaque `CMakeLists.txt` de sous-dossier liste explicitement les sources du projet nécessaires (ex.: `src/graph.c`, `src/tarjan.c`, etc.).
- Le projet racine définit `DATA_DIR` et `OUT_DIR` avant `add_subdirectory(test)`, ce qui rend ces macros visibles dans les sous-dossiers. Si vous déplacez `data/` ou `out/`, modifiez-les dans `CMakeLists.txt` à la racine.
