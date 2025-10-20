// ========================= utils.c =========================
//  - get_id_alpha : convertir les chiffre en lettre exemple : 1->A, 26->Z, 27->AA, etc.
//  - near_one     : vérifier si une somme est proche de 1
//  - ensure_dir   : créer récursivement les dossiers d'un chemin pour éviter toute erreur lors de l'ouverture

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <math.h>

#include "utils.h"

// -----------------------------------------------------------------------------
//                      Fonction : get_id_alpha
// Rôle : 1->"A", 26->"Z", 27->"AA", 28->"AB", ...
// Retourne un pointeur vers un buffer statique (réécrit à chaque appel).
// -----------------------------------------------------------------------------

const char *get_id_alpha(int k) {
    static char buf[16];    // tampon de sortie statique partagé entre les appels
    char tmp[16];           // construit à l'envers puis inversé dans buf
    int pos = 0;

    if (k <= 0) {           // garde-fou : l'API assume k>=1
        strcpy(buf, "?");   // Utile pour le debug
        return buf;
    }

    // Conversion "base 26 Excel" (pas de zéro) :
    // à chaque tour : r = (k-1) % 26 => lettre = 'A'+r ; k = (k-1) / 26
    while (k > 0 && pos < (int)sizeof(tmp) - 1) {
        int r = (k - 1) % 26;
        tmp[pos++] = (char)('A' + r);
        k = (k - 1) / 26;
    }
    tmp[pos] = '\0';  // termine tmp à l'envers

    // Inversion tmp -> buf
    for (int i = 0; i < pos; ++i) {
        buf[i] = tmp[pos - 1 - i];
    }
    buf[pos] = '\0';
    return buf;
}

// -----------------------------------------------------------------------------
//                      Fonction : near_one
// Rôle : vérifier si on est proche de 1
// retourne 1 si s ∈ [1-eps, 1+eps], sinon 0
// -----------------------------------------------------------------------------

int near_one(float s, float eps) {
    if (eps < 0.0f) eps = -eps;        // tolère un eps négatif en entrée
    return fabsf(s - 1.0f) <= eps;     // distance à 1 inférieure au seuil ?
}

// -----------------------------------------------------------------------------
//                      Fonction : ensure_dir
// Rôle : crée récursivement chaque composant d'un chemin de dossiers.
// Retourne 0 si OK, -1 en cas d'échec (errno est positionné).
// Best-effort : si le dossier existe (EEXIST), on continue.
// IMPORTANT : passer un chemin de dossier (ex: dirname(outfile)).
// -----------------------------------------------------------------------------

int ensure_dir(const char *path) {
    if (!path || !*path) {
        // Rien à faire : chaîne nulle ou vide => considérer OK
        return 0;
    }

    // On duplique 'path' car dirname/strtok modifient la chaîne.
    char *copy = strdup(path);
    if (!copy) return -1;

    // Enlever les slashs de fin (ex: "out///" -> "out")
    size_t len = strlen(copy);
    while (len > 1 && copy[len - 1] == '/') {
        copy[--len] = '\0';
    }

    // Cas particulier : si après nettoyage, il reste juste "/"
    if (strcmp(copy, "/") == 0) {
        free(copy);
        return 0; // la racine existe toujours
    }

    // Construire le chemin partiel au fur et à mesure.
    char partial[4096];
    partial[0] = '\0';

    // Si chemin absolu, on part de "/"
    if (copy[0] == '/') {
        strcpy(partial, "/");
    }

    // Découper le chemin sur les '/'
    char *tok = strtok(copy, "/");
    while (tok) {
        // Vérifier qu'on ne dépasse pas la taille de 'partial'
        size_t need = strlen(partial) + (partial[0] && strcmp(partial, "/") != 0 ? 1 : 0)
                    + strlen(tok) + 1; // +1 pour '\0'
        if (need >= sizeof(partial)) {
            free(copy);
            errno = ENAMETOOLONG;
            return -1;
        }

        // Ajouter "/" si nécessaire (sauf si partial est "/" déjà)
        if (partial[0] && strcmp(partial, "/") != 0) {
            strcat(partial, "/");
        }
        // Ajouter le segment courant
        strcat(partial, tok);

        // Tenter de créer ce niveau ; ignorer si déjà existant
        if (mkdir(partial, 0777) != 0) {
            if (errno != EEXIST) {
                // Erreur réelle (permissions, etc.)
                free(copy);
                return -1;
            }
        }

        tok = strtok(NULL, "/");
    }

    free(copy);
    return 0;
}
