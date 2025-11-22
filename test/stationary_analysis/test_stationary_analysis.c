#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "scc.h"
#include "graph.h"

static const float EPS = 1e-6f;

static void check_int_equal(const char *label, int got, int expected, int *failures)
{
    if (got == expected) {
        printf("  [OK]   %s (attendu=%d, obtenu=%d)\n", label, expected, got);
    } else {
        printf("  [FAIL] %s (attendu=%d, obtenu=%d)\n", label, expected, got);
        (*failures)++;
    }
}

static void check_float_equal(const char *label, float got, float expected, int *failures)
{
    float diff = got - expected;
    if (diff < 0) diff = -diff;
    if (diff < EPS) {
        printf("  [OK]   %s (attendu=%.4f, obtenu=%.4f)\n", label, (double)expected, (double)got);
    } else {
        printf("  [FAIL] %s (attendu=%.4f, obtenu=%.4f)\n", label, (double)expected, (double)got);
        (*failures)++;
    }
}

static void test_submatrix_basic_two_classes(int *failures)
{
    printf("\n--- TEST 1 : deux classes basiques {1,2} et {3,4} ---\n");

    t_matrix M = mx_zeros(4);
    const float data[4][4] = {
        {0.5f, 0.5f, 0.0f, 0.0f},
        {0.2f, 0.3f, 0.5f, 0.0f},
        {0.0f, 0.0f, 0.6f, 0.4f},
        {0.0f, 0.0f, 0.2f, 0.8f}
    };
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            M.a[i][j] = data[i][j];
        }
    }

    Partition P;
    scc_init_partition(&P);

    SccClass C1 = scc_make_empty_class();
    scc_add_vertex(&C1, 1);
    scc_add_vertex(&C1, 2);

    SccClass C2 = scc_make_empty_class();
    scc_add_vertex(&C2, 3);
    scc_add_vertex(&C2, 4);

    scc_add_class(&P, C1);
    scc_add_class(&P, C2);

    t_matrix S = subMatrix(M, P, 1);
    printf("  Sous-matrice C2={3,4} :\n");
    mx_print(&S);

    check_int_equal("Taille sous-matrice", S.n, 2, failures);
    check_float_equal("S[0][0]", S.a[0][0], 0.6f, failures);
    check_float_equal("S[0][1]", S.a[0][1], 0.4f, failures);
    check_float_equal("S[1][0]", S.a[1][0], 0.2f, failures);
    check_float_equal("S[1][1]", S.a[1][1], 0.8f, failures);

    mx_free(&M);
    mx_free(&S);
    scc_free_partition(&P);
}

static void test_submatrix_singleton(int *failures)
{
    printf("\n--- TEST 2 : une seule classe {2} ---\n");

    t_matrix M = mx_zeros(3);
    const float data[3][3] = {
        {1.0f, 0.1f, 0.2f},
        {0.3f, 0.4f, 0.5f},
        {0.6f, 0.7f, 0.8f}
    };
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            M.a[i][j] = data[i][j];
        }
    }

    Partition P;
    scc_init_partition(&P);

    SccClass C = scc_make_empty_class();
    scc_add_vertex(&C, 2);
    scc_add_class(&P, C);

    t_matrix S = subMatrix(M, P, 0);
    printf("  Sous-matrice C={2} :\n");
    mx_print(&S);

    check_int_equal("Taille sous-matrice", S.n, 1, failures);
    check_float_equal("S[0][0]", S.a[0][0], 0.4f, failures);

    mx_free(&M);
    mx_free(&S);
    scc_free_partition(&P);
}

static void test_submatrix_unsorted_class(int *failures)
{
    printf("\n--- TEST 3 : classe {4,2} (ordre non trié) ---\n");

    t_matrix M = mx_zeros(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            M.a[i][j] = (float)(10 * (i + 1) + (j + 1));
        }
    }

    Partition P;
    scc_init_partition(&P);

    SccClass C = scc_make_empty_class();
    scc_add_vertex(&C, 4);
    scc_add_vertex(&C, 2);
    scc_add_class(&P, C);

    t_matrix S = subMatrix(M, P, 0);
    printf("  Sous-matrice C={4,2} :\n");
    mx_print(&S);

    check_int_equal("Taille sous-matrice", S.n, 2, failures);
    check_float_equal("S[0][0]", S.a[0][0], 44.0f, failures);
    check_float_equal("S[0][1]", S.a[0][1], 42.0f, failures);
    check_float_equal("S[1][0]", S.a[1][0], 24.0f, failures);
    check_float_equal("S[1][1]", S.a[1][1], 22.0f, failures);

    mx_free(&M);
    mx_free(&S);
    scc_free_partition(&P);
}

static void test_submatrix_invalid_index(int *failures)
{
    printf("\n--- TEST 4 : compo_index invalide ---\n");

    t_matrix M = mx_zeros(2);
    Partition P;
    scc_init_partition(&P);

    t_matrix S1 = subMatrix(M, P, -1);
    check_int_equal("S1 taille (indice -1)", S1.n, 0, failures);
    check_int_equal("S1 pointeur (indice -1)", S1.a == NULL, 1, failures);

    t_matrix S2 = subMatrix(M, P, 0);
    check_int_equal("S2 taille (indice 0, partition vide)", S2.n, 0, failures);
    check_int_equal("S2 pointeur (indice 0, partition vide)", S2.a == NULL, 1, failures);

    mx_free(&M);
    scc_free_partition(&P);
}

static void test_submatrix_empty_class(int *failures)
{
    printf("\n--- TEST 5 : classe vide dans la partition ---\n");

    t_matrix M = mx_zeros(3);
    Partition P;
    scc_init_partition(&P);

    SccClass C = scc_make_empty_class();
    scc_add_class(&P, C);

    t_matrix S = subMatrix(M, P, 0);
    check_int_equal("Taille sous-matrice", S.n, 0, failures);
    check_int_equal("Pointeur sous-matrice", S.a == NULL, 1, failures);

    mx_free(&M);
    scc_free_partition(&P);
}

int main(void)
{
    printf("=== TEST Partie 3.2 : stationary-analysis (subMatrix) ===\n");

    int failures = 0;

    test_submatrix_basic_two_classes(&failures);
    test_submatrix_singleton(&failures);
    test_submatrix_unsorted_class(&failures);
    test_submatrix_invalid_index(&failures);
    test_submatrix_empty_class(&failures);

    if (failures > 0) {
        printf("\n=> ❌ %d test(s) échoué(s).\n", failures);
        return EXIT_FAILURE;
    }

    printf("\n=> ✅ Tous les tests subMatrix ont réussi.\n");
    return EXIT_SUCCESS;
}
