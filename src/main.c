#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // exit, strtof
#include <string.h>     // strcmp

#include "io.h"           // read_graph_from_file
#include "verify.h"       // verify_markov
#include "mermaid.h"      // export_mermaid
#include "graph.h"        // AdjList, graph_free
#include "tarjan.h"       // tarjan_partition
#include "hasse.h"        // build_class_links, remove_transitive_links
#include "mermaid_hasse.h"// export_hasse_mermaid
#include "markov_props.h" // markov_class_types, markov_is_irreducible...
#include "matrix.h"       // matrices + distributions
#include "period.h"       // class_period

// Structure des options de la ligne de commande
typedef struct {
    const char *infile;
    const char *out_graph;
    const char *out_hasse;
    float eps_markov;
    float eps_converge;
    int   keep_transitive;
    int   matrix_power;       // 0 = pas d'affichage
    int   converge_max_iter;  // pour diff(M^n, M^{n-1})
    int   dist_start;         // 0 = désactivé, sinon sommet de départ
    int   dist_steps;         // nb d'étapes pour la distribution
    int   do_stationary;
    int   do_period;
} Options;

// Affiche l'aide courte du programme --help
static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [options]\n\n"
        "Options principales:\n"
        "  --in FILE           Graphe d'entrée (format: N puis lignes 'from to proba')\n"
        "  --eps E             Tolérance Markov et convergences (def 0.01)\n"
        "  --out-graph FILE    Export Mermaid du graphe complet\n"
        "  --out-hasse FILE    Export Mermaid du diagramme de Hasse (classes)\n"
        "  --keep-transitive   Ne pas retirer les liens transitifs du Hasse\n"
        "  --matrix-power K    Affiche la matrice M^K\n"
        "  --converge-max N    Iter max pour diff(M^n, M^{n-1}) < eps (def 30)\n"
        "  --dist-start V --dist-steps T   Distribution après T étapes depuis le sommet V\n"
        "  --no-stationary     Ne pas calculer les distributions stationnaires par classe\n"
        "  --period            Calcule la période de chaque classe\n"
        "  --help              Afficher cette aide et quitter\n\n"
        "Exemple:\n"
        "  %s --in data/exemple_valid_step3.txt --out-graph out/mermaid/graph.mmd --out-hasse out/mermaid/hasse.mmd --matrix-power 3 --period\n",
        prog, prog);
}

// Puissance entière simple: out = M^k (k>=1)
static void mx_power_int(const t_matrix *M, int k, t_matrix *out) {
    if (!M || !M->a || M->n <= 0 || k < 1 || !out) return;
    t_matrix acc = mx_zeros(M->n);
    t_matrix tmp = mx_zeros(M->n);
    mx_copy(M, &acc);
    for (int step = 2; step <= k; ++step) {
        mx_mul(&acc, M, &tmp);
        mx_copy(&tmp, &acc);
    }
    mx_copy(&acc, out);
    mx_free(&acc);
    mx_free(&tmp);
}

// Affiche les classes (SCC)
static void print_partition(const Partition *p) {
    printf("[Partition] %d classe(s)\n", p->count);
    for (int i = 0; i < p->count; ++i) {
        printf("  C%d: {", i + 1);
        for (int j = 0; j < p->classes[i].count; ++j) {
            if (j) printf(", ");
            printf("%d", p->classes[i].verts[j]);
        }
        printf("}\n");
    }
}

// Affiche les liens Hasse (0-basé en interne, affichage 1-basé)
static void print_links(const HasseLinkArray *links) {
    if (!links) return;
    printf("[Hasse] %d lien(s)\n", links->count);
    for (int i = 0; i < links->count; ++i) {
        printf("  C%d -> C%d\n", links->links[i].from_class + 1, links->links[i].to_class + 1);
    }
}

// Parse les arguments de la ligne de commande
static int parse_args(int argc, char **argv, Options *opt) {
    // valeurs par défaut
    opt->infile          = DATA_DIR "/exemple_valid_step3.txt";
    opt->out_graph       = NULL;
    opt->out_hasse       = NULL;
    opt->eps_markov      = 0.01f;
    opt->eps_converge    = 0.01f;
    opt->keep_transitive = 0;
    opt->matrix_power    = 0;
    opt->converge_max_iter = 30;
    opt->dist_start      = 0;
    opt->dist_steps      = 0;
    opt->do_stationary   = 1;
    opt->do_period       = 0;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--in") && i + 1 < argc) {
            opt->infile = argv[++i];
        } else if (!strcmp(argv[i], "--eps") && i + 1 < argc) {
            opt->eps_markov = strtof(argv[++i], NULL);
            opt->eps_converge = opt->eps_markov;
        } else if (!strcmp(argv[i], "--out-graph") && i + 1 < argc) {
            opt->out_graph = argv[++i];
        } else if (!strcmp(argv[i], "--out-hasse") && i + 1 < argc) {
            opt->out_hasse = argv[++i];
        } else if (!strcmp(argv[i], "--keep-transitive")) {
            opt->keep_transitive = 1;
        } else if (!strcmp(argv[i], "--matrix-power") && i + 1 < argc) {
            opt->matrix_power = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--converge-max") && i + 1 < argc) {
            opt->converge_max_iter = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--dist-start") && i + 1 < argc) {
            opt->dist_start = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--dist-steps") && i + 1 < argc) {
            opt->dist_steps = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--no-stationary")) {
            opt->do_stationary = 0;
        } else if (!strcmp(argv[i], "--period")) {
            opt->do_period = 1;
        } else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "[ERR] Unknown or incomplete argument: %s\n", argv[i]);
            usage(argv[0]);
            return -1;
        }
    }
    return 1;
}

// Programme principal (argc: nombre d'arguments, argv: liste des arguments)
int main(int argc, char **argv) {
    Options opt;
    int parse_ok = parse_args(argc, argv, &opt);
    if (parse_ok <= 0) {
        return parse_ok == 0 ? 0 : 1;
    }

    // 1) Lecture du graphe depuis le fichier
    AdjList g;
    read_graph_from_file(opt.infile, &g);

    // 2) Vérification Markov (sommes sortantes ~ 1)
    int ok = verify_markov(&g, opt.eps_markov);  // Retourne 1 si ok, 0 sinon
    if (ok) {
        printf("[OK] Graphe valide (Markov) avec eps=%.4f\n", (double)opt.eps_markov);
    } else {
        printf("[WARN] Graphe NON valide (Markov) avec eps=%.4f — voir messages ci-dessus.\n", (double)opt.eps_markov);
    }

    // 3) Matrice de transition (Partie 3.1)
    t_matrix M = mx_from_adjlist(&g);

    // 4) Partition SCC (Tarjan) et liens de Hasse (Partie 2)
    Partition P;
    scc_init_partition(&P);
    tarjan_partition(&g, &P);
    print_partition(&P);

    HasseLinkArray links;
    hasse_init_links(&links);
    build_class_links(&g, &P, &links);
    if (!opt.keep_transitive && P.count > 0) {
        remove_transitive_links(&links, P.count);
    }
    print_links(&links);

    // 5) Typage des classes et propriétés Markov (Partie 2.3)
    int nb_classes = P.count;
    int *is_transient = NULL;
    int *is_persistent = NULL;
    if (nb_classes > 0) {
        is_transient = calloc((size_t)nb_classes, sizeof(int));
        is_persistent = calloc((size_t)nb_classes, sizeof(int));
        markov_class_types(&links, nb_classes, is_transient, is_persistent);
    }

    printf("[Classes] transitoire/persistante:\n");
    for (int i = 0; i < nb_classes; ++i) {
        printf("  C%d: %s\n", i + 1, is_persistent[i] ? "persistante" : "transitoire");
    }

    int irreducible = markov_is_irreducible(&P);
    printf("[Graphe] %s\n", irreducible ? "Irréductible (1 classe)" : "Non irréductible");

    printf("[Absorbants] ");
    int found_abs = 0;
    for (int v = 1; v <= g.size; ++v) {
        if (markov_is_absorbing_vertex(&P, &links, v)) {
            printf("%d ", v);
            found_abs = 1;
        }
    }
    if (!found_abs) printf("aucun");
    printf("\n");

    // 6) Exports Mermaid (Partie 1 et Partie 2)
    if (opt.out_graph) {
        if (export_mermaid(&g, opt.out_graph) == 0) {
            printf("[OK] Export Mermaid (graphe) -> %s\n", opt.out_graph);
        } else {
            fprintf(stderr, "[ERR] Échec de l'export Mermaid vers %s\n", opt.out_graph);
        }
    }
    if (opt.out_hasse) {
        if (export_hasse_mermaid(&P, &links, opt.out_hasse) == 0) {
            printf("[OK] Export Mermaid (Hasse) -> %s\n", opt.out_hasse);
        } else {
            fprintf(stderr, "[ERR] Échec de l'export Hasse vers %s\n", opt.out_hasse);
        }
    }

    // 7) Matrices : puissance fixée et convergence diff(M^n, M^(n-1)) < eps
    if (opt.matrix_power > 0) {
        t_matrix MP = mx_zeros(M.n);
        mx_power_int(&M, opt.matrix_power, &MP);
        printf("[Matrix] M^%d :\n", opt.matrix_power);
        mx_print(&MP);
        mx_free(&MP);
    }

    if (opt.converge_max_iter > 0) {
        t_matrix Mc = mx_zeros(M.n);
        int steps = 0;
        int conv = mx_power_until_diff(&M, opt.eps_converge, opt.converge_max_iter, &Mc, &steps);
        printf("[Matrix] Recherche convergence diff(M^n, M^{n-1}) < %.4f (max %d itérations)\n",
               (double)opt.eps_converge, opt.converge_max_iter);
        printf("  -> %s (n=%d)\n", conv == 1 ? "Atteint" : "Non atteint", steps);
        mx_free(&Mc);
    }

    // 8) Distribution après T étapes depuis un sommet donné
    if (opt.dist_steps > 0 && opt.dist_start >= 1 && opt.dist_start <= g.size) {
        float *pi0 = calloc((size_t)g.size, sizeof(float));
        float *pit = calloc((size_t)g.size, sizeof(float));
        if (pi0 && pit) {
            pi0[opt.dist_start - 1] = 1.0f;
            dist_power(pi0, &M, opt.dist_steps, pit);
            printf("[Distribution] après %d étape(s) en partant de %d : [", opt.dist_steps, opt.dist_start);
            for (int i = 0; i < g.size; ++i) {
                printf("%s%.4f", (i ? ", " : ""), (double)pit[i]);
            }
            printf("]\n");
        }
        free(pi0);
        free(pit);
    }

    // 9) Distributions stationnaires par classe persistante (Partie 3.2)
    if (opt.do_stationary && nb_classes > 0) {
        printf("[Stationnaire] Par classe (persistante => distribution limite, transitoire => 0)\n");
        for (int k = 0; k < nb_classes; ++k) {
            t_matrix sub = subMatrix(M, P, k);
            printf("  C%d: ", k + 1);
            if (!is_persistent[k] || sub.n == 0) {
                printf("transitoire -> [");
                for (int j = 0; j < sub.n; ++j) {
                    printf("%s0.0", (j ? ", " : ""));
                }
                printf("]\n");
            } else {
                float *pi = calloc((size_t)sub.n, sizeof(float));
                int conv = stationary_distribution(&sub, opt.eps_converge, opt.converge_max_iter, pi);
                printf("persistante -> [");
                for (int j = 0; j < sub.n; ++j) {
                    printf("%s%.4f", (j ? ", " : ""), (double)pi[j]);
                }
                printf("] (%s)\n", conv ? "converge" : "non convergé");
                free(pi);
            }
            mx_free(&sub);
        }
    }

    // 10) Période des classes (défi bonus Part 3.3)
    if (opt.do_period && nb_classes > 0) {
        printf("[Période] Par classe (via sous-matrice)\n");
        for (int k = 0; k < nb_classes; ++k) {
            t_matrix sub = subMatrix(M, P, k);
            int per = (sub.n > 0) ? class_period(&sub) : 0;
            printf("  C%d: période = %d\n", k + 1, per);
            mx_free(&sub);
        }
    }

    // Libération des ressources
    free(is_transient);
    free(is_persistent);
    hasse_free_links(&links);
    scc_free_partition(&P);
    mx_free(&M);
    graph_free(&g);

    // Code de retour : 0 si Markov OK, 2 si non-Markov, 1 si erreur d’arguments (déjà géré).
    return ok ? 0 : 2;
}
