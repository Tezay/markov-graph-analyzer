# Interface Web - Markov Graph Analyzer

> #### Module optionnel : le projet C peut fonctionner sans cette interface.

## Objectif
- Saisir facilement les arguments du binaire C (chemins d’entrée/sortie, options Hasse/Matricielles/Stationnaires/Période).
- Saisir un graphe à la main (format texte Partie 1) sans créer de fichier à la main : un fichier temporaire est généré et passé à `--in`.
- Lancer l’exécutable C facilement depuis un l'interface (accès à `data/` et `out/`).

## Usage rapide
1. Créez un venv, installez Flask et lancez le serveur :
   ```bash
   # Pour aller à la racine du projet (si lancé depuis webui/ avec CLion)
   cd ..
   python3 -m venv .venv
   . .venv/bin/activate  # Windows : .venv\Scripts\activate
   pip install flask
   FLASK_APP=webui/app.py flask run
   ```
   puis ouvrez `http://127.0.0.1:5000`. Le binaire C doit être construit et accessible (ex: `./markov_graph_analyzer` ou `./cmake-build-debug/markov_graph_analyzer`).

## Structure
```
webui/
├── app.py               # Serveur Flask, exécute le binaire C avec les arguments saisis
├── README.md
├── templates/
│    └── index.html      # Interface principale avec formulaire (Tailwind CDN)
└── static/
     └── js/
          └── script.js  # Gestion des tabs et éditeur de graphe simple
```

## Champs principaux / modes
- Chemin du binaire (ex : `./markov_graph_analyzer` ou `./cmake-build-debug/markov_graph_analyzer`).
- Modes exclusifs pour `--in` :
  - Fichier texte (onglet "Fichier").
  - Texte brut (onglet "Texte brut" → fichier temporaire).
  - Éditeur de graphe (onglet "Éditeur" → fichier temporaire à partir des arêtes).
- Autres options : `--out-graph`, `--out-hasse`, `--keep-transitive`, `--matrix-power`, `--converge-max`, `--dist-start`, `--dist-steps`, `--no-stationary`, `--period`.

## Capture d'écran

![Web UI](../assets/screens/webui.png)


## Comment c'est relié au projet
- L’interface web appelle le binaire C via `subprocess.run` (cwd = racine du projet).
- Les modes de saisie sont exclusifs et génèrent un fichier temporaire si besoin pour `--in`.
- N’altère pas le projet C : outil de confort uniquement.
- Certaines parties UI (Tailwind) et Javascript ont été générées avec l'assistance d'IAs génératives, n'étant pas le cœur du projet (optionnel).
