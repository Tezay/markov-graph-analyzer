#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#include "mermaid_hasse.h"
#include "scc.h"
#include "hasse.h"
#include "utils.h"


/**
 * @brief  Écrit l'en-tête YAML Mermaid pour le diagramme Hasse
 *
 * Configure le layout, le thème et le type de diagramme (flowchart LR)
 * pour rester cohérent avec le style de la Partie 1.
 *
 * @param[in]  f  Fichier déjà ouvert en écriture
 */
static void write_mermaid_header_hasse(FILE *f) {
    fputs("---\n", f);
    fputs("config:\n", f);
    fputs("   layout: elk\n", f);
    fputs("   theme: neo\n", f);
    fputs("   look: neo\n", f);
    fputs("---\n\n", f);
    fputs("flowchart LR\n", f); // même style que export_mermaid (Partie 1)
}

/**
 * @brief  Écrit un nœud Mermaid pour une classe Ck avec la liste des sommets
 *
 * Produit une ligne du type: Ck["Ck: {v1, v2, ...}"].
 *
 * @param[in]  f          Fichier déjà ouvert en écriture
 * @param[in]  class_idx  Index k de la classe (0..p->count-1)
 * @param[in]  cls        Pointeur vers la classe (SccClass)
 */
static void write_class_label(FILE *f, int class_idx, const SccClass *cls) {
    int cid = class_idx + 1; // affichage à partir de 1
    // Préfixe du nœud
    fprintf(f, "C%d[\"", cid);

    // Titre "Ck: {"
    fprintf(f, "C%d: {", cid);

    // Liste des sommets : v1, v2, ..., vk
    for (int j = 0; j < cls->count; ++j) {
        fprintf(f, "%d", cls->verts[j]);
        if (j + 1 < cls->count) {
            fputs(", ", f);
        }
    }

    // Fermeture "}]"
    fputs("}\"]\n", f);
}

/**
 * @brief  Écrit toutes les arêtes Mermaid entre classes
 *
 * Pour chaque lien (from_class, to_class), écrit une arête:
 *   Cfrom --> Cto
 *
 * @param[in]  f      Fichier déjà ouvert en écriture
 * @param[in]  links  Tableau des liens entre classes (HasseLinkArray)
 */
static void write_links(FILE *f, const HasseLinkArray *links) {
    if (!links || links->count <= 0) return;

    for (int i = 0; i < links->count; ++i) {
        int from = links->links[i].from_class + 1; // affichage à partir de 1
        int to   = links->links[i].to_class + 1;
        // On écrit les liens tels quels ; si B retire la transitivité, on aura un Hasse net.
        fprintf(f, "C%d --> C%d\n", from, to);
    }
}

/**
 * @brief  Exporte le graphe des classes (Partition + liens) au format Mermaid
 *
 * Chaque classe devient un nœud "Ck" dont le label affiche la liste de ses
 * sommets. Les relations entre classes deviennent des flèches Mermaid.
 *
 * @param[in]  p        Partition (liste des classes SCC)
 * @param[in]  links    Liens entre classes (voir hasse.h)
 * @param[in]  outfile  Chemin du .mmd de sortie (ex: "out/hasse.mmd")
 *
 * @return  0 si succès, -1 en cas d’erreur (arguments, répertoire, ouverture)
 *
 */
int export_hasse_mermaid(const Partition *p, const HasseLinkArray *links, const char *outfile) {
    // 0) Vérification des arguments
    if (!p || !outfile) {
        fprintf(stderr, "[export_hasse_mermaid] invalid arguments (p/outfile)\n");
        return -1;
    }

    // 1) S'assurer que le répertoire de sortie existe (même logique que Partie 1)
    char *dup = strdup(outfile);
    if (!dup) {
        perror("strdup(outfile)");
        return -1;
    }
    char *dir = dirname(dup);
    if (dir && strcmp(dir, ".") != 0) {
        if (ensure_dir(dir) != 0) {
            fprintf(stderr, "[export_hasse_mermaid] cannot create dir '%s' (errno=%d)\n", dir, errno);
            free(dup);
            return -1;
        }
    }
    free(dup);

    // 2) Ouvrir le fichier .mmd
    FILE *f = fopen(outfile, "wt");
    if (!f) {
        perror("fopen(outfile)");
        return -1;
    }

    // 3) Écrire l'en-tête Mermaid (même style et layout que ton export Partie 1)
    write_mermaid_header_hasse(f);

    // 4) Déclarer tous les nœuds Ck avec leurs labels "{...}"
    for (int k = 0; k < p->count; ++k) {
        const SccClass *cls = &p->classes[k];
        write_class_label(f, k, cls);
    }
    fputc('\n', f);

    // 5) Écrire toutes les arêtes entre classes
    if (links && links->count > 0) {
        write_links(f, links);
    }

    // 6) Fermer proprement
    if (fclose(f) != 0) {
        perror("fclose(outfile)");
        return -1;
    }

    // OK
    return 0;
}

/**
 * @brief  Exporte la partition seule en texte brut (utile pour le debug)
 *
 * Produit un fichier texte listant chaque classe et ses sommets :
 *  - En-tête: "# Partition (N classes)"
 *  - Puis une ligne par classe: "Class k: v1 v2 v3 ..."
 *
 * @param[in]  p        Partition (liste des classes SCC)
 * @param[in]  outfile  Chemin du .txt de sortie
 *
 * @return  0 si succès, -1 sinon (arguments, répertoire, ouverture)
 *
 */
int export_partition_text(const Partition *p, const char *outfile) {
    if (!p || !outfile) {
        fprintf(stderr, "[export_partition_text] invalid arguments (p/outfile)\n");
        return -1;
    }

    // Créer le dossier si besoin
    char *dup = strdup(outfile);
    if (!dup) {
        perror("strdup(outfile)");
        return -1;
    }
    char *dir = dirname(dup);
    if (dir && strcmp(dir, ".") != 0) {
        if (ensure_dir(dir) != 0) {
            fprintf(stderr, "[export_partition_text] cannot create dir '%s' (errno=%d)\n", dir, errno);
            free(dup);
            return -1;
        }
    }
    free(dup);

    // Ouvrir le fichier texte
    FILE *f = fopen(outfile, "wt");
    if (!f) {
        perror("fopen(outfile)");
        return -1;
    }

    // En-tête lisible
    fprintf(f, "# Partition (%d classes)\n", p->count);

    // Pour chaque classe, lister ses sommets
    for (int k = 0; k < p->count; ++k) {
        const SccClass *cls = &p->classes[k];
        fprintf(f, "Class %d:", k + 1); // Affichage à partir de 1
        for (int j = 0; j < cls->count; ++j) {
            fprintf(f, " %d", cls->verts[j]);
        }
        fputc('\n', f);
    }

    // Fermer proprement
    if (fclose(f) != 0) {
        perror("fclose(outfile)");
        return -1;
    }

    return 0;
}
