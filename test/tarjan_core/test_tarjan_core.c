#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "scc.h"
#include "tarjan.h"
#include "io.h"

static void print_partition(const Partition *p) {
    printf("Partition (%d classes):\n", p->count);
    for (int i = 0; i < p->count; ++i) {
        printf("  C%d {", i + 1);
        for (int j = 0; j < p->classes[i].count; ++j) {
            if (j) putchar(',');
            printf("%d", p->classes[i].verts[j]);
        }
        printf("}\n");
    }
}

static int cmp_int(const void *a, const void *b) {
    int ia = *(const int*)a, ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int main(void) {
    printf("=== TEST Partie 2.1 : tarjan_core (SCC/partition) ===\n");
    AdjList g;

    const char *path = "data/exemple_valid_step3.txt";
    read_graph_from_file(path, &g);

    Partition P;
    scc_init_partition(&P);
    tarjan_partition(&g, &P);

    print_partition(&P);

    // Construire la table sommet -> classe
    int class_of[9]; // 1..8
    memset(class_of, 0, sizeof(class_of));
    for (int k = 0; k < P.count; ++k) {
        for (int j = 0; j < P.classes[k].count; ++j) {
            int v = P.classes[k].verts[j];
            if (v >= 1 && v <= 8) class_of[v] = k;
        }
    }

    scc_free_partition(&P);
    graph_free(&g);
    printf("[OK] Partition Tarjan exécutée.\n");
    printf("=== FIN TEST tarjan_core ===\n");
    return 0;
}
