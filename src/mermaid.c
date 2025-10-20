// Export du graphe au format Mermaid (flowchart LR + config en-tête)

#include <stdio.h>      // FILE, fopen, fprintf, fclose, perror
#include <stdlib.h>     // malloc, free
#include <string.h>     // strcmp, strdup
#include <errno.h>      // errno
#include <libgen.h>     // dirname (POSIX)

#include "mermaid.h"   // export_mermaid
#include "utils.h"     // ensure_dir, get_id_alpha
#include "graph.h"     // AdjList, List, Cell

// Ecrit l'en-tête YAML/Front-matter attendu par Mermaid
static void write_mermaid_header(FILE *f) {
    fputs("---\n", f);
    fputs("config:\n", f);
    fputs("   layout: elk\n", f);
    fputs("   theme: neo\n", f);
    fputs("   look: neo\n", f);
    fputs("---\n\n", f);
    fputs("flowchart LR\n", f);
}

int export_mermaid(const AdjList *g, const char *outfile) {
    if (!g || !outfile) {
        fprintf(stderr, "[export_mermaid] invalid arguments (g/outfile)\n");
        return -1;
    }

    // S'assurer que le répertoire de sortie existe
    char *dup = strdup(outfile);
    if (!dup) {
        perror("strdup(outfile)");
        return -1;
    }
    char *dir = dirname(dup); // peut retourner "." si pas de répertoire
    if (dir && strcmp(dir, ".") != 0) {
        if (ensure_dir(dir) != 0) {
            fprintf(stderr, "[export_mermaid] cannot create dir '%s' (errno=%d)\n", dir, errno);
            free(dup);
            return -1;
        }
    }
    free(dup);

    FILE *f = fopen(outfile, "wt");
    if (!f) {
        perror("fopen(outfile)");
        return -1;
    }

    // En-tête et ligne de type
    write_mermaid_header(f);

    // Déclarer tous les nœuds (A((1)), B((2)), ...)
    for (int i = 1; i <= g->size; ++i) {
        const char *id = get_id_alpha(i);
        fprintf(f, "%s((%d))\n", id, i);
    }
    fputc('\n', f);

    // Déclarer toutes les arêtes avec l'étiquette de probabilité
    for (int from = 1; from <= g->size; ++from) {
        const char *from_id = get_id_alpha(from);
        for (Cell *c = g->array[from - 1].head; c; c = c->next) {
            int to = c->dest;
            const char *to_id = get_id_alpha(to);
            fprintf(f, "%s -->|%.2f| %s\n", from_id, (double)c->proba, to_id);
        }
    }

    if (fclose(f) != 0) {
        perror("fclose(outfile)");
        return -1;
    }
    return 0;
}