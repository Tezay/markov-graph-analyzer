<div id="top">

<!-- HEADER STYLE: CLASSIC -->
<div align="center">


# MARKOV-GRAPH-ANALYZER

<em>Projet d'étude Informatique et Mathématiques</em>

<!-- BADGES -->

<img src="assets/badges/school-efrei.svg" alt="School: EFREI Paris">
<img src="assets/badges/cohort-p2.svg" alt="Cohort: Prépa P2 2025–2026">
<img src="assets/badges/license-private.svg" alt="License: Private">

<img src="https://img.shields.io/badge/CMake-064F8C.svg?style=flat&logo=CMake&logoColor=white" alt="CMake">
<img src="https://img.shields.io/badge/C-A8B9CC.svg?style=flat&logo=C&logoColor=black" alt="C">

</div>
<br>

---

## Table des matières

- **[Aperçu](#overview)**
- **[Fonctionnalités](#features)**
- **[Structure du projet](#project-structure)**
- **[Prise en main](#getting-started)**
    - **[Prérequis](#prerequisites)**
    - **[Installation](#installation)**
    - **[Utilisation](#usage)**
    - **[Tests](#testing)**

---

## Aperçu <a id="overview"></a>

Ensemble d’outils pour analyser, valider et visualiser des graphes et chaînes de Markov. Combine des algorithmes avec une visualisation intuitive pour aider développeurs et chercheurs à comprendre des structures complexes.

- **🔍 Validation du graphe :** garantit l’intégrité probabiliste et la cohérence des chaînes de Markov.
- **🌐 Export Mermaid :** génère des diagrammes clairs et stylés pour comprendre la structure des graphes.
- **🧮 Composantes fortement connexes :** implémente l’algorithme de Tarjan pour la détection de cycles et la décomposition du graphe.
- **🧩 Architecture modulaire :** favorise des tests et validations extensifs pour un développement fiable.
- **📊 Analyse approfondie :** outils pour explorer classes de transition, structures cycliques et propriétés de graphe.

---

## Fonctionnalités <a id="features"></a>

|      | Composant       | Détails                                                                                     |
| :--- | :-------------- | :------------------------------------------------------------------------------------------ |
| ⚙️  | **Architecture**  | <ul><li>Conception modulaire séparant algorithmes cœur, I/O et utilitaires</li><li>Utilise CMake pour la configuration de build</li></ul> |
| 📄 | **Documentation** | <ul><li>README de base avec vue d’ensemble</li><li>Contient la documentation des CMakeLists.txt</li></ul> |
| 🧩 | **Tests**       | <ul><li>Sous-répertoires `test/` avec CMakeLists dédiés</li><li>Inclut des tests unitaires des algorithmes cœur (ex. algorithme de Tarjan)</li></ul> |
| ⚡️  | **Performance**   | <ul><li>Algorithmes de graphe optimisés en C</li></ul> |
| 📦 | **Dépendances**  | <ul><li>Dépendances externes minimales ; bibliothèques C standard principalement</li><li>Dépendances de build gérées via CMake</li></ul> |

---

## Structure du projet <a id="project-structure"></a>

```
└── markov-graph-analyzer/
    ├── CMakeLists.txt
    ├── README.md
    ├── include
    │   ├── graph.h
    │   ├── io.h
    │   ├── list.h
    │   ├── mermaid.h
    │   ├── scc.h
    │   ├── tarjan.h
    │   ├── utils.h
    │   ├── hasse.h
    │   ├── markov_props.h
    │   ├── mermaid_hasse.h
    │   ├── matrix.h
    │   ├── period.h
    │   └── verify.h
    ├── src
    │   ├── graph.c
    │   ├── io.c
    │   ├── list.c
    │   ├── main.c
    │   ├── mermaid.c
    │   ├── scc.c
    │   ├── tarjan.c
    │   ├── utils.c
    │   ├── hasse.c
    │   ├── markov_props.c
    │   ├── mermaid_hasse.c
    │   ├── matrix.c
    │   └── verify.c
    └── test
        ├── CMakeLists.txt
        ├── README.md
        ├── core
        ├── io_verify
        ├── mermaid_cli
        ├── tarjan_core
        ├── hasse_links
        ├── class_analysis_and_export
        └── matrix_ops
```

---

## Prise en main <a id="getting-started"></a>

### Prérequis <a id="prerequisites"></a>

- **Langage de programmation :** C
- **Outil de build :** CMake

### Installation <a id="installation"></a>

Construire markov-graph-analyzer depuis les sources et installer les dépendances :

#### 1. **Cloner le dépôt :**

  ```
  git clone https://github.com/Tezay/markov-graph-analyzer
  ```

#### 2. **Aller dans le répertoire du projet :**

  ```
  cd markov-graph-analyzer
  ```

#### 3. **Compiler :**

**Avec [CMake](https://isocpp.org/):**

```
cmake . && make
```

### Utilisation <a id="usage"></a>

Exécuter le projet :

**Avec [CMake](https://isocpp.org/):**

```
./markov-graph-analyzer
```

### Tests <a id="testing"></a>

- **Guide des tests : [test/README.md](test/README.md)**
