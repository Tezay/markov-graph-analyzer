#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // exit, strtof
#include <string.h>     // strcmp

#include "io.h"        // read_graph_from_file (Personne B)
#include "verify.h"    // verify_markov        (Personne B)
#include "mermaid.h"   // export_mermaid       (Personne C)
#include "graph.h"     // AdjList, graph_free  (Personne A)

// Affiche l'aide courte du programme --help
static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [--in FILE] [--out FILE] [--eps 0.01] [--help]\n\n"
        "Options:\n"
        "  --in FILE    Fichier d'entrée (format: N puis lignes 'from to proba')\n"
        "  --out FILE   Fichier de sortie Mermaid (.mmd)\n"
        "  --eps E      Tolérance de vérification Markov (par défaut 0.01)\n"
        "  --help       Afficher cette aide et quitter\n\n"
        "Exemple:\n  %s --in data/exemple1.txt --out out/graph.mmd --eps 0.01\n",
        prog, prog);
}

int main(int argc, char **argv) {
    // Valeurs par défaut
    const char *infile  = "data/exemple1.txt"; // fichier attendu : N puis 'from to proba'
    const char *outfile = "out/graph.mmd";     // export Mermaid
    float eps = 0.01f;                          // tolérance de Markov

    // Lecture et traitement des arguments de la ligne de commande
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--in") && i + 1 < argc) {
            infile = argv[++i];
        } else if (!strcmp(argv[i], "--out") && i + 1 < argc) {
            outfile = argv[++i];
        } else if (!strcmp(argv[i], "--eps") && i + 1 < argc) {
            eps = strtof(argv[++i], NULL);
        } else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "[ERR] Unknown or incomplete argument: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    // 1) Lecture du graphe depuis le fichier
    AdjList g;                                   // structure du graphe
    read_graph_from_file(infile, &g);            // lève exit(EXIT_FAILURE) en cas d'erreur I/O/format

    // 2) Vérification Markov (sommes sortantes ~ 1)
    int ok = verify_markov(&g, eps);  // Retourne 1 si ok, 0 sinon
    if (ok) {
        printf("[OK] Graphe valide (Markov) avec eps=%.4f\n", (double)eps);
    } else {
        printf("[WARN] Graphe NON valide (Markov) avec eps=%.4f — voir messages ci-dessus.\n", (double)eps);
    }

    // 3) Export Mermaid (écrit les nœuds et arêtes formatés)
    if (export_mermaid(&g, outfile) == 0) {
        printf("[OK] Export Mermaid -> %s\n", outfile);
    } else {
        fprintf(stderr, "[ERR] Échec de l'export Mermaid vers %s\n", outfile);
        // on continue pour libérer proprement
    }

    // 4) Libération des ressources du graphe
    graph_free(&g);                               // libère toutes les listes et le tableau interne

    // Code de retour : 0 si Markov OK, 2 si non-Markov (utile en CI/tests), 1 si erreur d’arguments.
    return ok ? 0 : 2;
}
