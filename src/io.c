#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "io.h"

/**
 * @brief  Supprime les espaces blancs en fin de chaîne (trim droit)
 *
 * Coupe la chaîne en place en remplaçant, à partir de la fin, les
 * caractères dont le code ASCII est inférieur ou égal à ' ' par un
 * '\0'. Si `s` vaut NULL, la fonction ne fait rien.
 *
 * @param[in,out] s  Chaîne C modifiée en place (peut être NULL)
 *
 * @return  void
 *
 * @warning Le pointeur doit référencer une zone mémoire modifiable.
 */
static void rtrim(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n > 0 && (unsigned char)s[n - 1] <= ' ') {
        s[--n] = '\0';
    }
}

/**
 * @brief  Indique si une ligne est vide ou un commentaire
 *
 * Ignore les espaces en tête. Considère comme commentaire/ligne vide
 * toute ligne vide, toute ligne commençant par '#', ou par '//' (après
 * retrait des espaces de tête).
 *
 * @param[in] s  Pointeur sur la ligne C terminée par '\0'
 *
 * @return  1 si la ligne est vide/commentaire, 0 sinon
 *
 * @pre     `s` doit être non NULL.
 */
static int is_comment_or_blank(const char *s) {
    // ignore lignes vides, commentaires # ou //
    while (*s && isspace((unsigned char)*s)) ++s;
    if (*s == '\0') return 1;
    if (*s == '#')  return 1;
    if (*s == '/' && *(s + 1) == '/') return 1;
    return 0;
}

/**
 * @brief  Lit un graphe depuis un fichier texte et le stocke dans `out`
 *
 * Le format attendu est:
 *   - une première ligne utile contenant `N` (nombre de sommets), après
 *     éventuels commentaires/espaces;
 *   - des lignes de la forme: `from to proba`, potentiellement entrecoupées
 *     de lignes vides ou de commentaires ("#" ou "//"). Les probabilités
 *     doivent être dans [0, 1] et les sommets dans [1, N]. Les lignes
 *     invalides sont signalées et ignorées.
 *
 * La fonction initialise `out` via `graph_init(out, N)` et ajoute les arêtes
 * valides avec `graph_add_edge`. En cas d'erreur d'ouverture du fichier ou
 * de `N` invalide, la fonction affiche un message et termine le programme
 * avec `exit(EXIT_FAILURE)`.
 *
 * @param[in]  filename  Chemin du fichier d'entrée (texte)
 * @param[out] out       Graphe de sortie initialisé et rempli
 *
 * @return  void
 *
 * @pre     `filename` et `out` doivent être non NULL.
 * @see     graph_init, graph_add_edge
 */
void read_graph_from_file(const char *filename, AdjList *out) {
    // Ouverture du fichier en lecture texte
    FILE *f = fopen(filename, "rt");
    if (!f) {
        perror("[IO] fopen");
        fprintf(stderr, "[IO][ERR] Impossible d'ouvrir '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    // Lecture de N : on saute lignes vides/commentées jusqu’à trouver un entier
    int n = -1;
    {
        // Variable pour stocker chaque ligne lue
        char line[256];
        // Boucle lecture ligne par ligne
        while (fgets(line, sizeof(line), f)) {
            // Vérifie si la ligne est un commentaire ou vide
            rtrim(line);
            if (is_comment_or_blank(line)) continue;
            // Vérifie si la ligne contient un entier
            if (sscanf(line, "%d", &n) == 1) break;
            fprintf(stderr, "[IO][ERR] Ligne invalide pour N: '%s'\n", line);
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    if (n <= 0) {
        fprintf(stderr, "[IO][ERR] Nombre de sommets N invalide (%d).\n", n);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    // Initialisation du graphe
    graph_init(out, n);

    // Lecture des triples : from to proba
    int from, to;
    float p;

    // Lecture ligne par ligne
    char buf[256];
    // Variable pour stocker le numéro de ligne pour messages d'erreur
    int lineno = 1;
    // Boucle de lecture des lignes restantes
    while (fgets(buf, sizeof(buf), f)) {
        ++lineno;
        rtrim(buf);
        if (is_comment_or_blank(buf)) continue;

        // matched stock un entier : nombre d’éléments lus par sscanf (0 à 3)
        int matched = sscanf(buf, "%d %d %f", &from, &to, &p);
        // Si 3 éléments lus
        if (matched == 3) {
            // Vérification que from et to ∈ [1 ; N]
            if (from < 1 || from > n || to < 1 || to > n) {
                fprintf(stderr, "[IO][ERR] L%d: sommet hors bornes: from=%d to=%d (1..%d)\n",
                        lineno, from, to, n);
                continue;
            }
            // vérification que p ∈ [0 ; 1]
            if (p < 0.0f || p > 1.0f) {
                fprintf(stderr, "[IO][ERR] L%d: probabilité invalide: %.6f pour %d->%d\n",
                        lineno, p, from, to);
                continue;
            }
            // Empile les arêtes dans le graphe
            graph_add_edge(out, from, to, p);
        } else {
            fprintf(stderr, "[IO][ERR] L%d: ligne invalide: '%s'\n", lineno, buf);
            continue;
        }
    }

    fclose(f);
}
