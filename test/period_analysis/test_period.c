#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "period.h"

static t_matrix create_matrix(int n, const float data[])
{
    t_matrix M = mx_zeros(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            M.a[i][j] = data[i * n + j];
        }
    }
    return M;
}

static void check_int_equal(const char *label, int got, int expected, int *fail_count)
{
    if (got == expected) {
        printf("  [OK]   %s (attendu=%d, obtenu=%d)\n", label, expected, got);
    } else {
        printf("  [FAIL] %s (attendu=%d, obtenu=%d)\n", label, expected, got);
        (*fail_count)++;
    }
}

static void run_case(const char *title,
                     const float *data,
                     int n,
                     int expected_period,
                     int expected_unique,
                     int *fail_count)
{
    printf("\n--- %s ---\n", title);

    t_matrix M = create_matrix(n, data);

    int period = class_period(&M);
    int has_unique = class_has_unique_stationary(&M);

    printf("  Matrice %dx%d:\n", n, n);
    mx_print(&M);

    check_int_equal("Période détectée", period, expected_period, fail_count);
    check_int_equal("Stationnaire unique", has_unique, expected_unique, fail_count);

    mx_free(&M);
}

int main(void)
{
    printf("=== TEST Partie 3.3 : period-analysis ===\n");

    int failures = 0;

    const float data_d3[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f
    };
    run_case("Cas 1 : Graphe fortement périodique (d=3)", data_d3, 3, 3, 0, &failures);

    const float data_d1[] = {
        0.5f, 0.5f,
        1.0f, 0.0f
    };
    run_case("Cas 2 : Graphe apériodique (d=1, auto-boucle)", data_d1, 2, 1, 1, &failures);

    const float data_d2[] = {
        0.0f, 1.0f,
        1.0f, 0.0f
    };
    run_case("Cas 3 : Graphe fortement périodique (d=2)", data_d2, 2, 2, 0, &failures);

    if (failures > 0) {
        printf("\n=> ❌ %d test(s) échoué(s).\n", failures);
        return EXIT_FAILURE;
    }

    printf("\n=> ✅ Tous les tests de période ont réussi.\n");
    return 0;
}
