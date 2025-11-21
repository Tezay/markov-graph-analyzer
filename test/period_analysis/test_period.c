#include <stdio.h>
#include <stdlib.h>
#include "matrix.h" 
#include "period.h"

#define ASSERT_EQ(actual, expected, message) \
    if ((actual) == (expected)) { \
        printf("  Passe: %s\n", (message)); \
    } else { \
        printf(" Passe Pas: %s. Attendu %d, Obtenu %d\n", (message), (expected), (actual)); \
        tests_failed++; \
    }

// Fonction utilitaire pour créer et remplir manuellement une matrice n x n
t_matrix create_matrix(int n, const float data[]) {
    t_matrix M;
    M.n = n;
    // Allocation de l'array de pointeurs
    M.a = (float**)malloc(n * sizeof(float*)); 
    for (int i = 0; i < n; i++) {
        M.a[i] = (float*)malloc(n * sizeof(float)); 
        for (int j = 0; j < n; j++) {
            M.a[i][j] = data[i * n + j];
        }
    }
    return M;
}

void test_period_cases(int *tests_failed) {
    
    printf("\n--- Cas 1: Graphe fortement périodique (d=3) ---\n");
    const float data_d3[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f
    };
    t_matrix M_d3 = create_matrix(3, data_d3);

    int period_d3 = class_period(&M_d3);
    ASSERT_EQ(period_d3, 3, "Période d=3 détectée");

    int is_unique_d3 = class_has_unique_stationary(&M_d3);
    ASSERT_EQ(is_unique_d3, 0, "Non apériodique (pas de stationnaire unique)");

    mx_free(&M_d3);

    printf("\n--- Cas 2: Graphe apériodique (d=1, auto-boucle) ---\n");
    

    const float data_d1[] = {
        0.5f, 0.5f,
        1.0f, 0.0f
    };
    t_matrix M_d1 = create_matrix(2, data_d1);

    int period_d1 = class_period(&M_d1);
    ASSERT_EQ(period_d1, 1, "Période d=1 détectée (apériodique)");

    int is_unique_d1 = class_has_unique_stationary(&M_d1);
    ASSERT_EQ(is_unique_d1, 1, "Apériodique (a une stationnaire unique)");

    mx_free(&M_d1);

    printf("\n--- Cas 3: Graphe fortement périodique (d=2) ---\n");

    const float data_d2[] = {
        0.0f, 1.0f,
        1.0f, 0.0f
    };
    t_matrix M_d2 = create_matrix(2, data_d2);

    int period_d2 = class_period(&M_d2);
    ASSERT_EQ(period_d2, 2, "Période d=2 détectée");

    int is_unique_d2 = class_has_unique_stationary(&M_d2);
    ASSERT_EQ(is_unique_d2, 0, "Non apériodique (période 2)");

    mx_free(&M_d2);
}

int main(void) {

    int tests_failed = 0;
    
    test_period_cases(&tests_failed);

    if (tests_failed > 0) {
        printf("\n=> ❌ %d test(s) échoué(s).\n", tests_failed);
        return EXIT_FAILURE;
    } else {
        printf("\n=> ✅ Tous les tests de période ont réussi.\n");
        return EXIT_SUCCESS;
    }
}