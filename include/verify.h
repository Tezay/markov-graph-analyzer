#ifndef VERIFY_H
#define VERIFY_H
#include "graph.h"

// Vérifie Markov: pour chaque sommet, somme des probas sortantes ≈ 1
// Retourne 1 si OK, 0 sinon. Affiche les erreurs détectées.
int verify_markov(const AdjList *g, float eps);

#endif