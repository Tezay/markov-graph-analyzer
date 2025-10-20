#ifndef GRAPH_H
#define GRAPH_H
#include "list.h"

typedef struct {
    int  size;     // nb de sommets
    List *array;   // tableau de listes: array[i] = sorties du sommet i+1
} AdjList;

// API graphe
void     graph_init(AdjList *g, int n);
void     graph_free(AdjList *g);
void     graph_add_edge(AdjList *g, int from, int to, float proba);
void     graph_print(const AdjList *g);

#endif