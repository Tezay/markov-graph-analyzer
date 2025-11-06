#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#include "mermaid_hasse.h"
#include "scc.h"
#include "hasse.h"
#include "utils.h"


// -----------------------------------------------------------------------------
//                      Fonction statique : write_mermaid_header_hasse
// Rôle : Écrire un en-tête YAML Mermaid cohérent avec ton style Partie 1
//        (layout, thème) + le type de diagramme (flowchart LR).
// -----------------------------------------------------------------------------
static void write_mermaid_header_hasse(FILE *f) {
    fputs("---\n", f);
    fputs("config:\n", f);
    fputs("   layout: elk\n", f);
    fputs("   theme: neo\n", f);
    fputs("   look: neo\n", f);
    fputs("---\n\n", f);
    fputs("flowchart LR\n", f); // même style que export_mermaid (Partie 1)
}

// -----------------------------------------------------------------------------
//                      Fonction statique : write_class_label
// Rôle : Écrire une ligne Mermaid pour un nœud de classe Ck avec la liste des
//        sommets entre accolades. Exemple :
//
//   C0["C0: {2,4,8,12}"]
//
// Entrées :
//   - f         : fichier déjà ouvert en écriture
//   - class_idx : index k de la classe (0..p->count-1)
//   - cls       : pointeur vers la classe (SccClass)
// -----------------------------------------------------------------------------
static void write_class_label(FILE *f, int class_idx, const SccClass *cls) {
    // Préfixe du nœud
    fprintf(f, "C%d[\"", class_idx);

    // Titre "Ck: {"
    fprintf(f, "C%d: {", class_idx);

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

// -----------------------------------------------------------------------------
//                      Fonction statique : write_links
// Rôle : Écrire toutes les arêtes Mermaid entre classes, au format :
//
//   Cfrom --> Cto
//
// Entrées :
//   - f     : fichier déjà ouvert
//   - links : tableau de liens entre classes
// -----------------------------------------------------------------------------
static void write_links(FILE *f, const HasseLinkArray *links) {
    if (!links || links->count <= 0) return;

    for (int i = 0; i < links->count; ++i) {
        int from = links->links[i].from_class;
        int to   = links->links[i].to_class;
        // On écrit les liens tels quels ; si B retire la transitivité, on aura un Hasse net.
        fprintf(f, "C%d --> C%d\n", from, to);
    }
}

// -----------------------------------------------------------------------------
//                      Fonction : export_hasse_mermaid
// Rôle : Exporter le graphe des classes (Partition + HasseLinkArray) au format
//        Mermaid (.mmd). Chaque classe devient un nœud "Ck", avec la liste de
//        ses sommets affichée dans le label. Les liens entre classes deviennent
//        des flèches.
//
// Entrées :
//   - p       : Partition (liste des classes SCC)
//   - links   : Liens entre classes (cf. hasse.h)
//   - outfile : chemin du .mmd de sortie (ex: "out/hasse.mmd")
//
// Retour :
//   - 0 si succès
//   - -1 si erreur (arguments, création dossier, ouverture fichier, etc.)
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
//                      Fonction : export_partition_text
// Rôle : Exporter la partition seule en texte brut (pratique pour debug).
//
// Format de sortie exemple :
//
//   # Partition (4 classes)
//   Class 0: 2 4 8 12
//   Class 1: 3
//   Class 2: 7
//   Class 3: 35
//
// Entrées :
//   - p       : Partition (liste des classes SCC)
//   - outfile : chemin du .txt de sortie
//
// Retour :
//   - 0 si succès
//   - -1 sinon
// -----------------------------------------------------------------------------
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
        fprintf(f, "Class %d:", k);
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
