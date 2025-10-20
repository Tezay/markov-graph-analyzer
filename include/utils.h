#ifndef UTILS_H
#define UTILS_H
#include "graph.h"

// renvoie "A","B",...,"Z","AA","AB",...
const char *get_id_alpha(int k);

// somme ≈ 1 dans [1-eps, 1+eps]
int near_one(float s, float eps);

// crée récursivement un répertoire si nécessaire (best-effort, ok si absent)
int ensure_dir(const char *path);


// Vérifie Markov: pour chaque sommet, somme des probas sortantes ≈ 1
// Retourne 1 si OK, 0 sinon. Affiche les erreurs détectées.
int verify_markov(const AdjList *g, float eps);

#endif