#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "matrix.h"

enum {
    STATE_CLOUDY = 1,
    STATE_RAINY  = 2,
    STATE_SUNNY  = 3
};

/**
 * @brief  Construit un petit graphe de météo (3 états)
 *
 * États :
 *  - 1 : Cloudy
 *  - 2 : Rainy
 *  - 3 : Sunny
 *
 * Chaque ligne sortante forme une distribution de probabilité qui somme à 1.
 * Ceci permet de tester la construction de la matrice de transition et
 * les puissances matricielles sur un exemple concret.
 *
 * @param[out] g  Graphe en liste d'adjacence initialisé par la fonction
 */
static void build_weather_graph(AdjList *g)
{
    graph_init(g, 3);

    /*
     * Cloudy -> {Cloudy, Rainy, Sunny}
     */
    graph_add_edge(g, STATE_CLOUDY, STATE_CLOUDY, 0.5f);
    graph_add_edge(g, STATE_CLOUDY, STATE_RAINY,  0.25f);
    graph_add_edge(g, STATE_CLOUDY, STATE_SUNNY,  0.25f);

    /*
     * Rainy -> {Cloudy, Rainy, Sunny}
     */
    graph_add_edge(g, STATE_RAINY, STATE_CLOUDY, 0.3f);
    graph_add_edge(g, STATE_RAINY, STATE_RAINY,  0.4f);
    graph_add_edge(g, STATE_RAINY, STATE_SUNNY,  0.3f);

    /*
     * Sunny -> {Cloudy, Rainy, Sunny}
     */
    graph_add_edge(g, STATE_SUNNY, STATE_CLOUDY, 0.2f);
    graph_add_edge(g, STATE_SUNNY, STATE_RAINY,  0.2f);
    graph_add_edge(g, STATE_SUNNY, STATE_SUNNY,  0.6f);
}

/**
 * @brief  Affiche un vecteur de distribution de taille n avec un label
 *
 * Chaque composante est affichée avec un format flottant uniforme.
 *
 * @param[in] label  Préfixe affiché avant les valeurs
 * @param[in] pi     Vecteur de distribution
 * @param[in] n      Taille du vecteur
 */
static void print_distribution(const char *label, const float *pi, int n)
{
    printf("%s [", label);
    for (int i = 0; i < n; ++i) {
        printf("%8.4f", (double)pi[i]);
        if (i + 1 < n) {
            printf(", ");
        }
    }
    printf("]\n");
}

/**
 * @brief  Calcule la puissance entière d'une matrice: out = M^k
 *
 * Pour k >= 1, effectue k-1 multiplications successives en utilisant `mx_mul`.
 * La matrice résultat est copiée dans `out`.
 *
 * @param[in]  M    Matrice de base (carrée)
 * @param[in]  k    Puissance entière (k >= 1)
 * @param[out] out  Matrice résultat
 */
static void mx_power_int(const t_matrix *M, int k, t_matrix *out)
{
    if (!M || M->n <= 0 || !M->a || k < 1) {
        fprintf(stderr, "[test_matrix_ops][ERR] Paramètres invalides pour mx_power_int\n");
        exit(EXIT_FAILURE);
    }

    t_matrix acc = mx_zeros(M->n);
    t_matrix tmp = mx_zeros(M->n);

    mx_copy(M, &acc);

    for (int step = 2; step <= k; ++step) {
        mx_mul(&acc, M, &tmp);
        mx_copy(&tmp, &acc);
    }

    mx_copy(&acc, out);

    mx_free(&acc);
    mx_free(&tmp);
}

/**
 * @brief  Test simple de convergence: diff(M^k, M^{k+1}) < eps
 *
 * Calcule successivement M^k puis M^{k+1} et affiche à chaque itération
 * la valeur de `mx_diff_abs1`. Stoppe si la différence devient inférieure
 * à `eps` ou si `max_iter` est atteint.
 *
 * @param[in] M         Matrice de transition de base
 * @param[in] max_iter  Nombre maximal d'itérations
 * @param[in] eps       Tolérance de convergence
 */
static void test_convergence(const t_matrix *M, int max_iter, float eps)
{
    t_matrix Mk = mx_zeros(M->n);
    t_matrix Mk1 = mx_zeros(M->n);

    mx_copy(M, &Mk);

    for (int k = 1; k <= max_iter; ++k) {
        mx_mul(&Mk, M, &Mk1);
        float diff = mx_diff_abs1(&Mk, &Mk1);
        printf("[conv] k=%d diff=%.6f\n", k, (double)diff);

        mx_copy(&Mk1, &Mk);

        if (diff < eps) {
            printf("[conv] Convergence atteinte pour k=%d (diff < %.6f)\n", k, (double)eps);
            break;
        }
    }

    mx_free(&Mk);
    mx_free(&Mk1);
}

/**
 * @brief  Programme de test pour la Partie 3.1 (matrix-ops)
 *
 * - construit un petit graphe météo
 * - génère la matrice de transition M
 * - affiche M, M^3 et M^7
 * - calcule des distributions \Pi_t pour Cloudy et Rainy
 * - effectue un test de convergence \|M^k - M^{k+1}\|_1 < eps
 */
int main(void)
{
    printf("=== TEST Partie 3.1 : matrix-ops (matrices & distributions) ===\n");

    AdjList g;
    build_weather_graph(&g);

    t_matrix M = mx_from_adjlist(&g);

    printf("\n[Matrice] M (transition météo) :\n");
    mx_print(&M);

    t_matrix M3 = mx_zeros(M.n);
    t_matrix M7 = mx_zeros(M.n);

    mx_power_int(&M, 3, &M3);
    mx_power_int(&M, 7, &M7);

    printf("\n[Matrice] M^3 :\n");
    mx_print(&M3);

    printf("\n[Matrice] M^7 :\n");
    mx_print(&M7);

    int n = M.n;
    float *pi0 = (float *)malloc((size_t)n * sizeof(float));
    float *pit = (float *)malloc((size_t)n * sizeof(float));
    if (!pi0 || !pit) {
        fprintf(stderr, "[test_matrix_ops][ERR] malloc pour les distributions\n");
        free(pi0);
        free(pit);
        graph_free(&g);
        mx_free(&M);
        mx_free(&M3);
        mx_free(&M7);
        return 1;
    }

    /* Distribution initiale: tout en Cloudy */
    for (int i = 0; i < n; ++i) {
        pi0[i] = 0.0f;
    }
    pi0[STATE_CLOUDY - 1] = 1.0f;

    dist_power(pi0, &M, 3, pit);
    print_distribution("Pi_t (t=3) en partant de Cloudy", pit, n);

    dist_power(pi0, &M, 7, pit);
    print_distribution("Pi_t (t=7) en partant de Cloudy", pit, n);

    /* Distribution initiale: tout en Rainy */
    for (int i = 0; i < n; ++i) {
        pi0[i] = 0.0f;
    }
    pi0[STATE_RAINY - 1] = 1.0f;

    dist_power(pi0, &M, 3, pit);
    print_distribution("Pi_t (t=3) en partant de Rainy", pit, n);

    dist_power(pi0, &M, 7, pit);
    print_distribution("Pi_t (t=7) en partant de Rainy", pit, n);

    /* Test de convergence diff(M^k, M^{k+1}) < eps */
    printf("\n[Test] Convergence des puissances M^k :\n");
    test_convergence(&M, 20, 1e-3f);

    free(pi0);
    free(pit);

    mx_free(&M);
    mx_free(&M3);
    mx_free(&M7);
    graph_free(&g);

    printf("\n=== FIN TEST matrix-ops ===\n");
    return 0;
}
