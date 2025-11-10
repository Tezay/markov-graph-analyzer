#ifndef HASSE_H
#define HASSE_H
#include "graph.h"
#include "scc.h"

typedef struct {
    int from_class;
    int to_class;
} HasseLink;

typedef struct {
    HasseLink *links;
    int        count;
    int        capacity;
} HasseLinkArray;

void build_vertex_to_class_map(Partition *p, int n_vertices, int *class_of_vertex);

void build_class_links(const AdjList *g, const Partition *p, HasseLinkArray *out_links);

void remove_transitive_links(HasseLinkArray *links, int nb_classes);

void hasse_init_links(HasseLinkArray *arr);
void hasse_free_links(HasseLinkArray *arr);
int  hasse_link_exists(const HasseLinkArray *arr, int c_from, int c_to);

#endif