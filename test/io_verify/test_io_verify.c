// Test unitiaires pour feature/io_verify (IO & Markov verif) sur tous les fichiers data/*.txt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "io.h"
#include "verify.h"
#include "graph.h"

#ifndef DATA_DIR
#error DATA_DIR doit être défini (par CMakeLists)
#endif

static const char *path_join(const char *fname) {
    static char buf[1024];
    snprintf(buf, sizeof(buf), "%s/%s", DATA_DIR, fname);
    return buf;
}

static int ends_with_txt(const char *name) {
    size_t n = strlen(name);
    return n >= 4 && strcmp(name + (n - 4), ".txt") == 0;
}

static int count_edges(const AdjList *g) {
    int m = 0;
    for (int i = 1; i <= g->size; ++i) {
        for (Cell *c = g->array[i - 1].head; c; c = c->next) m++;
    }
    return m;
}

int main(void) {
    DIR *dir = opendir(DATA_DIR);
    if (!dir) {
        perror("opendir(DATA_DIR)");
        return 1;
    }

    int total = 0;
    int ok_cnt = 0;
    int ko_cnt = 0;

    struct dirent *ent;
    // Parcourt tous les fichiers du répertoire DATA_DIR
    while ((ent = readdir(dir)) != NULL) {
        const char *name = ent->d_name;
        if (name[0] == '.') continue; // skip hidden/parent/current
        if (!ends_with_txt(name)) continue; // only *.txt

        total++;
        const char *filepath = path_join(name);
        printf("\n[TEST][%d] %s\n", total, filepath);

        AdjList g;
        read_graph_from_file(filepath, &g);

        int edges = count_edges(&g);
        printf("  - nodes=%d edges=%d\n", g.size, edges);

        int ok = verify_markov(&g, 0.01f);
        if (ok) {
            printf("  => [OK] Markov verification passed.\n");
            ok_cnt++;
        } else {
            printf("  => [KO] Markov verification failed.\n");
            ko_cnt++;
        }

        graph_free(&g);
    }
    closedir(dir);

    printf("\n[SUMMARY] files=%d ok=%d ko=%d\n", total, ok_cnt, ko_cnt);
    // Retourne le nombre de tests échoués
    return ko_cnt;
}
