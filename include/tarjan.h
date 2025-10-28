#ifndef TARJAN_H
#define TARJAN_H

#include "graph.h"
#include "scc.h"

// Struct pour stocker les informations de chaque sommet pendant l'algorithme de Tarjan
typedef struct {
    int id;        // 1..N
    int index;     // index de découverte, -1 si non défini
    int lowlink;   // plus petit index accessible
    int on_stack;  // booléen 0/1
} t_tarjan_vertex;

// Struct pour stocker le contexte de l'algorithme de Tarjan
typedef struct {
    const AdjList *g;
    Partition *out;
    t_tarjan_vertex *V; // [1..N]
    int *stack;         // pile d'ids
    int sp;             // taille courante de pile
    int cap;            // capacité pile
    int N;              // nombre de sommets
    int next_index;     // compteur d'index
} t_ctx;

// Lance Tarjan sur g et remplit 'out' avec la partition (ordre des classes indifférent)
void tarjan_partition(const AdjList *g, Partition *out);

#endif
