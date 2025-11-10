#ifndef UTILS_H
#define UTILS_H
#include "graph.h"
//include parti 2
#include "scc.h"
#include "hasse.h"

//Parti 1

// renvoie "A","B",...,"Z","AA","AB",...
const char *get_id_alpha(int k);

// somme ≈ 1 dans [1-eps, 1+eps]
int near_one(float s, float eps);

// crée récursivement un répertoire si nécessaire (best-effort, ok si absent)
int ensure_dir(const char *path);

#endif