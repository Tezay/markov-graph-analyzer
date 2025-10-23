#include <stdio.h>
#include "verify.h"

// Vérifie si x ∈ [a, b]
static int in_range(float x, float a, float b) {
    return x >= a && x <= b;
}

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