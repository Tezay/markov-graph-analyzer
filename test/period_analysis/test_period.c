#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#include "matrix.h"
#include "period.h"

/**
 * @brief Crée et initialise une matrice de transition de taille n x n.
 * @param n Taille de la matrice.
 * @param data Tableau 1D des données à placer dans la matrice.
 * @return t_matrix Matrice de transition initialisée.
 */
static t_matrix create_matrix(int n, const float data[])
{
    t_matrix M = mx_zeros(n);
    if (M.a == NULL) {
        fprintf(stderr, "Erreur d'allocation dans create_matrix.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Remplissage en utilisant l'indexation 1D: M[i][j] <-> data[i * n + j]
            M.a[i][j] = data[i * n + j];
        }
    }
    return M;
}

/**
 * @brief Vérifie l'égalité entre la valeur obtenue et la valeur attendue.
 */
static void check_int_equal(const char *label, int got, int expected, int *fail_count)
{
    if (got == expected) {
        printf("  [OK]   %s (attendu=%d, obtenu=%d)\n", label, expected, got);
    } else {
        printf("  [FAIL] %s (attendu=%d, obtenu=%d)\n", label, expected, got);
        (*fail_count)++;
    }
}

/**
 * @brief Exécute un cas de test complet pour la période et l'unicité stationnaire.
 */
static void run_case(const char *title,
                     const float *data,
                     int n,
                     int expected_period,
                     int expected_unique,
                     int *fail_count)
{
    printf("\n--- %s ---\n", title);

    t_matrix M = create_matrix(n, data);

    // 1. Calcul de la période et de l'unicité
    int period = class_period(&M);
    int has_unique = class_has_unique_stationary(&M);

    printf(" Matrice %dx%d:\n", n, n);
    // mx_print(&M); // Décommenter si la fonction d'affichage de matrice est disponible

    // 2. Vérification des résultats
    check_int_equal("Période détectée", period, expected_period, fail_count);
    // Vrai (1) si apériodique (période=1), Faux (0) sinon.
    check_int_equal("Stationnaire unique", has_unique, expected_unique, fail_count);

    mx_free(&M); // Libère la mémoire
}

int main(void)
{
    printf("=== TEST Partie 3.3 : period-analysis ===\n");

    int failures = 0;

    // --- CAS 1 : Période d=3 (Cycle 1 -> 2 -> 3 -> 1) ---
    // M = [[0, 1, 0], [0, 0, 1], [1, 0, 0]]
    // Un retour à l'état de départ n'est possible qu'après 3, 6, 9... étapes. PGCD = 3.
    const float data_d3[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f
    };
    // Période attendue = 3. Unique stationnaire attendu = 0.
    run_case("Cas 1 : Graphe fortement périodique (d=3)", data_d3, 3, 3, 0, &failures);

    // --- CAS 2 : Période d=1 (Apériodique) ---
    // M = [[0.5, 0.5], [1.0, 0.0]]
    // L'auto-boucle (0.5 sur l'état 1) permet le retour en 1, 2, 3... étapes. PGCD = 1.
    const float data_d1[] = {
        0.5f, 0.5f,
        1.0f, 0.0f
    };
    // Période attendue = 1. Unique stationnaire attendu = 1.
    run_case("Cas 2 : Graphe apériodique (d=1, auto-boucle)", data_d1, 2, 1, 1, &failures);

    // --- CAS 3 : Période d=2 (Cycle 1 <-> 2) ---
    // M = [[0, 1], [1, 0]]
    // Un retour n'est possible qu'après 2, 4, 6... étapes. PGCD = 2.
    const float data_d2[] = {
        0.0f, 1.0f,
        1.0f, 0.0f
    };
    // Période attendue = 2. Unique stationnaire attendu = 0.
    run_case("Cas 3 : Graphe fortement périodique (d=2)", data_d2, 2, 2, 0, &failures);

    if (failures > 0) {
        printf("\n=> ❌ %d test(s) échoué(s).\n", failures);
        return EXIT_FAILURE;
    }

    printf("\n=> ✅ Tous les tests de période ont réussi.\n");
    return 0;
}