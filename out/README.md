# Dossier de sortie des exports Mermaid

> Ce dossier contient les exports Mermaid générés par le binaire C ou les tests.

## Formats attendus
- **Graphe brut** (`--out-graph`) : fichier `.mmd` Mermaid de type `flowchart LR` avec nœuds `(i)` et arêtes `A -->|p| B` (style Mermaid Chart).
- **Diagramme de Hasse** (`--out-hasse`) : fichier `.mmd` Mermaid listant les classes `C1`, `C2`, ... et les liens inter-classes.
- **Exports texte** (certains tests) : partition listée en clair.

## Exemple d’export (graphe)
```mmd
---
config:
   layout: elk
   theme: neo
   look: neo
---

flowchart LR
A((1))
B((2))

A -->|0.50| A
A -->|0.50| B
B -->|1.00| A
```

## Utilisation sur mermaidchart.com
1. Ouvrir https://www.mermaidchart.com/.
2. Copier/coller le contenu du `.mmd` exporté dans l’éditeur.
3. Choisir le layout (ex: elk) et ajuster le thème si besoin.
4. Exporter en image/PDF depuis l’interface Mermaid Chart.

## Notes
- Les chemins d’export par défaut sont définis via `OUT_DIR` (CMake) pour les exports graphiques.
- Vous pouvez déplacer les `.mmd` où vous le souhaitez.
