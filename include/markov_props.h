#ifndef MARKOV_PROPS_H
#define MARKOV_PROPS_H
#include "scc.h"
#include "hasse.h"

// Remplit deux tableaux de bool (0/1) de taille #classes
void markov_class_types(const HasseLinkArray *links, int nb_classes,
                        int *is_transient, int *is_persistent);

// Retourne 1 si le graphe est irréductible (une seule classe), 0 sinon.
int markov_is_irreducible(const Partition *p);

// Retourne 1 si l'état v est absorbant (classe persistante de taille 1 et v l’unique)
int markov_is_absorbing_vertex(const Partition *p, const HasseLinkArray *links, int v);

#endif