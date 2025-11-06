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

//Parti 2
//Trouve dans qu'elle class le vertex ce trouve
int find_class_of_vertex(const Partition *p, int v);

//Savoir si la class a au moins 1 lien sortant
int class_has_outgoing_link(const HasseLinkArray *links, int class_idx);
#endif