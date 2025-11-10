#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "scc.h"
#include "tarjan.h"
#include "hasse.h"

static void print_partition(const Partition *p) {
    printf("=== Partition (SCCs) ===\n");
    for (int i = 0; i < p->count; i++) {
        printf("Classe %d: ", i);
        for (int j = 0; j < p->classes[i].count; j++) {
            printf("%d ", p->classes[i].verts[j]);
        }
        printf("\n");
    }
}

static void print_links(const HasseLinkArray *arr) {
    printf("=== Liens entre classes ===\n");
    for (int i = 0; i < arr->count; i++) {
        printf("Classe %d -> Classe %d\n", arr->links[i].from_class, arr->links[i].to_class);
    }
}

int main(void) {
    AdjList g;
    graph_init(&g, 6);

    // Exemple de graphe avec 6 sommets et 2 composantes fortement connexes
    graph_add_edge(&g, 1, 2, 1.0f);
    graph_add_edge(&g, 2, 3, 1.0f);
    graph_add_edge(&g, 3, 1, 1.0f);  // boucle 1–2–3

    graph_add_edge(&g, 4, 5, 1.0f);
    graph_add_edge(&g, 5, 4, 1.0f);  // boucle 4–5
    graph_add_edge(&g, 3, 4, 1.0f);  // lien entre les deux composantes
    graph_add_edge(&g, 5, 6, 1.0f);  // 6 isolé sinon

    printf("=== Graphe ===\n");
    graph_print(&g);

    Partition p;
    scc_init_partition(&p);
    tarjan_partition(&g, &p);
    print_partition(&p);

    HasseLinkArray links = {NULL, 0, 0};
    build_class_links(&g, &p, &links);
    print_links(&links);

    hasse_free_links(&links);
    scc_free_partition(&p);
    graph_free(&g);

    return 0;
}
