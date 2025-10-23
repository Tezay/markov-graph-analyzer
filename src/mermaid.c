#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#include "mermaid.h"
#include "utils.h"
#include "graph.h"


// Écrit dans un fichier l'en-tête YAML attendu par le format Mermaid pour configurer le style et la disposition du diagramme.

static void write_mermaid_header(FILE *f) {
    fputs("---\n", f);
    fputs("config:\n", f);
    fputs("   layout: elk\n", f);
    fputs("   theme: neo\n", f);
    fputs("   look: neo\n", f);
    fputs("---\n\n", f);
    fputs("flowchart LR\n", f);
}
// -----------------------------------------------------------------------------
//                      Fonction : export_mermaid
// Rôle : Exporte un graphe sous forme de fichier Mermaid (.mmd)
// Retourne 0 si succès, -1 si erreur (avec message d'erreur sur stderr)
// -----------------------------------------------------------------------------

int export_mermaid(const AdjList *g, const char *outfile) {
    // Vérifie la validité des arguments
    if (!g || !outfile) {
        fprintf(stderr, "[export_mermaid] invalid arguments (g/outfile)\n");
        return -1;
    }

    // S'assure que le répertoire du fichier de sortie existe
    char *dup = strdup(outfile);  // Duplique la chaîne du chemin pour pouvoir la modifier
    if (!dup) {                   // Vérifie si la duplication a échoué (mémoire insuffisante)
        perror("strdup(outfile)");
        return -1;
    }

    char *dir = dirname(dup);     // Extrait le répertoire à partir du chemin complet (ex: "out/graph.mmd" -> "out")
    if (dir && strcmp(dir, ".") != 0) { // Si le fichier n'est pas dans le répertoire courant
        if (ensure_dir(dir) != 0) {     // Crée le répertoire s’il n’existe pas
            fprintf(stderr, "[export_mermaid] cannot create dir '%s' (errno=%d)\n", dir, errno);
            free(dup);
            return -1;
        }
    }
    free(dup); // Libère la mémoire allouée à la copie du chemin

    // Ouvre le fichier de sortie en mode texte pour écriture
    FILE *f = fopen(outfile, "wt");
    if (!f) {                      // Si l’ouverture échoue (dossier inexistant, droits, etc.)
        perror("fopen(outfile)");
        return -1;
    }

    // Écrit l'en-tête Mermaid
    write_mermaid_header(f);


     // 1. Déclaration de tous les nœuds du graphe
    for (int i = 1; i <= g->size; ++i) {
        const char *id = get_id_alpha(i);      // Récupère un identifiant alphabétique (A, B, C, ...)
        fprintf(f, "%s((%d))\n", id, i);       // Écrit la ligne Mermaid correspondante
    }
    fputc('\n', f); // Ajoute une ligne vide pour la lisibilité


     // 2. Déclaration de toutes les arêtes (liaisons)

    for (int from = 1; from <= g->size; ++from) {
        // Copier l'identifiant source dans un buffer local
        char from_id[16];
        strncpy(from_id, get_id_alpha(from), sizeof(from_id));
        from_id[sizeof(from_id) - 1] = '\0';

        for (Cell *c = g->array[from - 1].head; c; c = c->next) { // Parcourt la liste des voisins
            int to = c->dest;                           // Nœud de destination
            // Copier l'identifiant destination dans un buffer local
            char to_id[16];
            strncpy(to_id, get_id_alpha(to), sizeof(to_id));
            to_id[sizeof(to_id) - 1] = '\0';

            fprintf(f, "%s -->|%.2f| %s\n", from_id, (double)c->proba, to_id); // Écrit la connexion avec la probabilité
        }
    }

    // Ferme le fichier proprement
    if (fclose(f) != 0) {
        perror("fclose(outfile)");
        return -1;
    }

    // Succès
    return 0;
}
