#ifndef VERIFY_H
#define VERIFY_H

#include "graph.h"

// Vérifie que pour chaque sommet i, la somme des probabilités sortantes ∈ [1-eps, 1+eps].
// Retourne 1 si tout est OK, 0 sinon.
// Affiche des messages d'erreur détaillés pour diagnostics.
int verify_markov(const AdjList *g, float eps);

#endif