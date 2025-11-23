#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>

/* ====== Headers du projet ====== */
#include "graph.h"     // AdjList, List, ...
#include "list.h"      // Cell (si défini ici chez toi)
#include "utils.h"     // get_id_alpha, near_one, ensure_dir
#include "mermaid.h"   // export_mermaid

/* ─────────────────────────────────────────────────────────────────────────── */
/* Helpers de gestion de graphe (mocks internes)                              */
/* ─────────────────────────────────────────────────────────────────────────── */


static void print_adjlist(const AdjList *g, const char *title) {
    if (title) printf("=== %s ===\n", title);
    for (int i = 1; i <= g->size; ++i) {
        printf("[%s](%d):", get_id_alpha(i), i);
        for (Cell *c = g->array[i - 1].head; c; c = c->next) {
            printf(" -> (%d, %.2f)", c->dest, (double)c->proba);
        }
        puts("");
    }
}

/* ─────────────────────────────────────────────────────────────────────────── */
/* Fonctions attendues par main.c / mermaid.c (mocks A/B)                     */
/* ─────────────────────────────────────────────────────────────────────────── */

int verify_markov(const AdjList *g, float eps) {
    if (!g) return 0;
    int ok = 1;
    for (int i = 1; i <= g->size; ++i) {
        float s = 0.0f;
        for (Cell *c = g->array[i - 1].head; c; c = c->next) {
            s += c->proba;
        }
        if (!near_one(s, eps)) {
            fprintf(stderr, "[MARKOV] Sommet %d: somme=%.4f (eps=%.4f) -> NON valide\n",
                    i, (double)s, (double)eps);
            ok = 0;
        }
    }
    if (ok) {
        printf("[MARKOV] Le graphe est un graphe de Markov (eps=%.4f)\n", (double)eps);
    } else {
        printf("[MARKOV] Le graphe n'est PAS un graphe de Markov (eps=%.4f)\n", (double)eps);
    }
    return ok;
}

static int fread_int(FILE *f, int *x) { return fscanf(f, "%d", x) == 1; }
static int fread_edge(FILE *f, int *a, int *b, float *p) { return fscanf(f, "%d %d %f", a, b, p) == 3; }

void read_graph_from_file(const char *filename, AdjList *g) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror("Impossible d'ouvrir le fichier en lecture");
        exit(EXIT_FAILURE);
    }
    int nb;
    if (!fread_int(file, &nb)) {
        perror("Impossible de lire le nombre de sommets");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    graph_init(g, nb);
    int from, to;
    float p;
    while (fread_edge(file, &from, &to, &p)) {
        graph_add_edge(g, from, to, p);
    }
    fclose(file);
}

/* ─────────────────────────────────────────────────────────────────────────── */
/* Utilitaires de test                                                        */
/* ─────────────────────────────────────────────────────────────────────────── */

static int file_exists_nonempty(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return (st.st_size > 0);
}

static void write_text_file(const char *path, const char *content) {
    if (ensure_dir("data") != 0) {
        fprintf(stderr, "[ERR] ensure_dir(data) a échoué\n");
        exit(EXIT_FAILURE);
    }
    FILE *f = fopen(path, "wt");
    if (!f) { perror("fopen(write_text_file)"); exit(EXIT_FAILURE); }
    fputs(content, f);
    fclose(f);
}

/* ─────────────────────────────────────────────────────────────────────────── */
/* Jeux de données                                                            */
/* ─────────────────────────────────────────────────────────────────────────── */

static void make_sample_graph_valid(AdjList *g) {
    graph_init(g, 4);
    graph_add_edge(g, 1, 1, 0.95f);
    graph_add_edge(g, 1, 2, 0.04f);
    graph_add_edge(g, 1, 3, 0.01f);

    graph_add_edge(g, 2, 2, 0.90f);
    graph_add_edge(g, 2, 3, 0.05f);
    graph_add_edge(g, 2, 4, 0.05f);

    graph_add_edge(g, 3, 3, 0.80f);
    graph_add_edge(g, 3, 4, 0.20f);

    graph_add_edge(g, 4, 1, 1.00f);
}

static void make_sample_graph_bad(AdjList *g) {
    graph_init(g, 4);
    graph_add_edge(g, 1, 1, 0.95f);
    graph_add_edge(g, 1, 2, 0.04f);
    graph_add_edge(g, 1, 3, 0.01f);

    graph_add_edge(g, 2, 2, 0.90f);
    graph_add_edge(g, 2, 3, 0.05f);
    graph_add_edge(g, 2, 4, 0.05f);

    graph_add_edge(g, 3, 3, 0.77f); // volontairement faux (0.77 + 0.20 = 0.97)
    graph_add_edge(g, 3, 4, 0.20f);

    graph_add_edge(g, 4, 1, 1.00f);
}

/* ─────────────────────────────────────────────────────────────────────────── */
/* Tests unitaires                                                            */
/* ─────────────────────────────────────────────────────────────────────────── */

static void test_get_id_alpha(void) {
    printf("\n[TEST] get_id_alpha\n");
    const char *a = get_id_alpha(1);
    const char *z = get_id_alpha(26);
    char aa[8], ab[8];
    strncpy(aa, get_id_alpha(27), sizeof(aa)); aa[sizeof(aa)-1] = '\0';
    strncpy(ab, get_id_alpha(28), sizeof(ab)); ab[sizeof(ab)-1] = '\0';

    printf("1 -> %s (attendu A)\n", a);
    printf("26 -> %s (attendu Z)\n", z);
    printf("27 -> %s (attendu AA)\n", aa);
    printf("28 -> %s (attendu AB)\n", ab);
}

static void test_near_one(void) {
    printf("\n[TEST] near_one\n");
    float eps = 0.01f;
    printf("near_one(1.000, 0.01) = %d (attendu 1)\n", near_one(1.000f, eps));
    printf("near_one(0.993, 0.01) = %d (attendu 0)\n", near_one(0.993f, eps));
    printf("near_one(1.008, 0.01) = %d (attendu 1)\n", near_one(1.008f, eps));
    printf("near_one(1.020, 0.01) = %d (attendu 0)\n", near_one(1.020f, eps));
}

static void test_ensure_dir(void) {
    printf("\n[TEST] ensure_dir\n");
    int r = ensure_dir("out/sub/dir");
    printf("ensure_dir(\"out/sub/dir\") -> %d (0 attendu)\n", r);
}

static void test_export_mermaid_memory_graphs(void) {
    printf("\n[TEST] export_mermaid (graphiques en mémoire)\n");
    AdjList g1, g2;
    make_sample_graph_valid(&g1);
    make_sample_graph_bad(&g2);

    ensure_dir("out");

    const char *f1 = "out/test_valid.mmd";
    const char *f2 = "out/test_bad.mmd";

    int e1 = export_mermaid(&g1, f1);
    int e2 = export_mermaid(&g2, f2);

    printf("export_mermaid(valide) -> %d | fichier=%s | existe=%d\n", e1, f1, file_exists_nonempty(f1));
    printf("export_mermaid(invalide)   -> %d | fichier=%s | existe=%d\n", e2, f2, file_exists_nonempty(f2));

    graph_free(&g1);
    graph_free(&g2);
}

static void test_verify_markov(void) {
    printf("\n[TEST] verify_markov (vérification Markov)\n");
    AdjList g1, g2;
    make_sample_graph_valid(&g1);
    make_sample_graph_bad(&g2);

    int ok1 = verify_markov(&g1, 0.01f);
    int ok2 = verify_markov(&g2, 0.01f);

    printf("verify_markov(valide)   -> %d (attendu 1)\n", ok1);
    printf("verify_markov(invalide) -> %d (attendu 0)\n", ok2);

    graph_free(&g1);
    graph_free(&g2);
}

static void test_read_graph_from_file_and_export(void) {
    printf("\n[TEST] lecture de fichier + export_mermaid\n");

    const char *path = "data/exemple1.txt";
    const char *content =
        "4\n"
        "1 1 0.95\n"
        "1 2 0.04\n"
        "1 3 0.01\n"
        "2 2 0.9\n"
        "2 3 0.05\n"
        "2 4 0.05\n"
        "3 3 0.8\n"
        "3 4 0.2\n"
        "4 1 1\n";
    write_text_file(path, content);

    AdjList g;
    read_graph_from_file(path, &g);
    print_adjlist(&g, "Liste d'adjacence lue depuis fichier");

    const char *outf = "out/test_file.mmd";
    ensure_dir("out");
    int e = export_mermaid(&g, outf);

    printf("export_mermaid(fichier) -> %d | fichier=%s | existe=%d\n", e, outf, file_exists_nonempty(outf));
    int ok = verify_markov(&g, 0.01f);
    printf("verify_markov(fichier) -> %d (attendu 1)\n", ok);

    graph_free(&g);
}

/* ─────────────────────────────────────────────────────────────────────────── */
/* Point d'entrée                                                             */
/* ─────────────────────────────────────────────────────────────────────────── */
#ifndef TEST_NO_MAIN
int main(void) {
    printf("=== TEST Partie 1.3 : mermaid_cli (utils + export) ===\n");

    test_get_id_alpha();
    test_near_one();
    test_ensure_dir();
    test_export_mermaid_memory_graphs();
    test_verify_markov();
    test_read_graph_from_file_and_export();

    printf("\n=== FIN TEST mermaid_cli ===\n");
    return 0;
}
#endif
