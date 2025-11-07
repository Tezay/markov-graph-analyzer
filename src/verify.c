#include <stdio.h>
#include "verify.h"

/**
 * @brief  Teste si une valeur appartient à [a, b] (inclus)
 *
 * Compare `x` à l'intervalle fermé [a, b] et renvoie 1 si `a <= x <= b`,
 * sinon 0.
 *
 * @param[in] x  Valeur testée
 * @param[in] a  Borne inférieure de l'intervalle
 * @param[in] b  Borne supérieure de l'intervalle
 *
 * @return  1 si x ∈ [a, b], 0 sinon
 *
 * @pre     On suppose `a <= b`.
 */
static int in_range(float x, float a, float b) {
    return x >= a && x <= b;
}

/**
 * @brief  Vérifie la propriété de chaîne de Markov du graphe
 *
 * Pour chaque sommet `i`, vérifie que la somme des probabilités des
 * transitions sortantes est comprise dans [1 - eps, 1 + eps]. Vérifie
 * également que chaque probabilité est dans [0, 1] et que chaque
 * destination est un sommet valide de `g`.
 *
 * En cas d'incohérence, affiche des messages d'erreur explicites sur la
 * sortie d'erreur standard. Si tout est correct, affiche un message de
 * succès sur la sortie standard.
 *
 * @param[in] g    Pointeur vers le graphe d'adjacence (initialisé)
 * @param[in] eps  Tolérance absolue (>= 0) pour la somme à 1
 *
 * @return  1 si toutes les vérifications passent, 0 sinon
 *
 * @pre     `g` doit être non NULL, `g->size > 0`, `g->array` non NULL.
 * @see     AdjList, List, Cell
 */
int verify_markov(const AdjList *g, float eps) {
    if (!g || g->size <= 0 || !g->array) {
        fprintf(stderr, "[Markov][ERR] Graphe invalide (structure non initialisée).\n");
        return 0;
    }

    int ok = 1;

    // Intération sur chaque sommet de la liste d'adjacence
    for (int i = 1; i <= g->size; ++i) {
        // Initialisation de la somme des probabilités sortantes
        float sum = 0.0f;
        // Récupération de la liste des sorties du sommet i
        List *lst = &g->array[i - 1];

        // Itération sur chaque cellule de la liste
        for (Cell *c = lst->head; c; c = c->next) {
            // Vérification de la probabilité (comprise entre 0 et 1)
            if (c->proba < 0.0f || c->proba > 1.0f) {
                fprintf(stderr, "[Markov][ERR] Sommet %d: probabilité invalide %.6f vers %d\n",
                        i, c->proba, c->dest);
                ok = 0;
            }
            // Vérification de la destination (doit être un sommet valide)
            if (c->dest < 1 || c->dest > g->size) {
                fprintf(stderr, "[Markov][ERR] Sommet %d: destination hors bornes %d (1..%d)\n",
                        i, c->dest, g->size);
                ok = 0;
            }

            // Incrémente la somme des probabilités
            sum += c->proba;
        }

        // Vérification que la somme des probabilités sortantes = 1 (+- eps)
        if (!in_range(sum, 1.0f - eps, 1.0f + eps)) {
            fprintf(stderr, "[Markov][ERR] Sommet %d: somme=%.6f hors intervalle [%.2f, %.2f]\n", i, sum, 1.0f - eps, 1.0f + eps);
            ok = 0;
        }
    }

    // Message de succès si tout est correct
    if (ok) {
        printf("[Markov][OK] Toutes les lignes sortantes somment à 1 (eps=%.3f).\n", eps);
    }
    return ok;
}
