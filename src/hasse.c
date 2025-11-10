#include <stdlib.h>
#include <stdio.h>
#include "hasse.h"
#include "graph.h"
#include "scc.h"

void hasse_init_links(HasseLinkArray *arr){
    arr->links = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

void hasse_free_links(HasseLinkArray *arr){
    free(arr->links);
    arr->capacity = 0;
    arr->count = 0;
}
//**
// * @brief 
// * 
// * @param arr 
// * @param c_from 
// * @param c_to 
// * @return int 
 // */
int hasse_link_exists(const HasseLinkArray *arr, int c_from, int c_to){
    for (int i = 0; i<arr->count; i++) {
        if(arr->links[i].from_class == c_from-1 && arr->links[i].to_class == c_to-1){
            return 1;
        }
    }
    return 0;
}

void build_vertex_to_class_map(Partition *p, int n_vertices, int *class_of_vertex){
    for(int i =0; i<p->count;i++){
        SccClass v = p->classes[i];
        for (int j = 0; j<v.count; j++) {
            int k = v.verts[i];
            if (k >= 1 && k <= n_vertices) {
                class_of_vertex[k] = j;
            }
        }
    }
}

void build_class_links(const AdjList *g, const Partition *p, HasseLinkArray *out_links) {
    int *class_of_vertex = malloc((g->size + 1) * sizeof(int));
    build_vertex_to_class_map((Partition *)p, g->size, class_of_vertex);

    for (int v = 1; v <= g->size; v++) {
        for (Cell *adj = g->array[v - 1].head; adj != NULL; adj = adj->next) {
            int from_class = class_of_vertex[v];
            int to_class = class_of_vertex[adj->dest];
            if (from_class != to_class && !hasse_link_exists(out_links, from_class + 1, to_class + 1)) {
                if (out_links->count == out_links->capacity) {
                    out_links->capacity = out_links->capacity == 0 ? 4 : out_links->capacity * 2;
                    out_links->links = realloc(out_links->links, out_links->capacity * sizeof(HasseLink));
                }
                out_links->links[out_links->count].from_class = from_class;
                out_links->links[out_links->count].to_class = to_class;
                out_links->count++;
            }
        }
    }

    free(class_of_vertex);
}



void remove_transitive_links(HasseLinkArray *links, int nb_classes){
    int **reach = malloc(nb_classes * sizeof(int *));
    for (int i = 0; i < nb_classes; i++) {
        reach[i] = calloc(nb_classes, sizeof(int));
    }

    for (int i = 0; i < links->count; i++) {
        reach[links->links[i].from_class][links->links[i].to_class] = 1;
    }

    for (int k = 0; k < nb_classes; k++) {
        for (int i = 0; i < nb_classes; i++) {
            for (int j = 0; j < nb_classes; j++) {
                if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
            }
        }
    }

    int write_idx = 0;
    for (int i = 0; i < links->count; i++) {
        int from = links->links[i].from_class;
        int to = links->links[i].to_class;
        int transitive = 0;
        for (int k = 0; k < nb_classes; k++) {
            if (k != from && k != to && reach[from][k] && reach[k][to]) {
                transitive = 1;
                break;
            }
        }
        if (!transitive) {
            links->links[write_idx++] = links->links[i];
        }
    }
    links->count = write_idx;

    for (int i = 0; i < nb_classes; i++) free(reach[i]);
    free(reach);
}
