#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "scc.h"
#include "matrix.h"

/**
 * @brief  Alloue une matrice n×n initialisée à zéro
 *
 * @param[in] n  Taille de la matrice (nombre de lignes et colonnes)
 *
 * @return  Une structure `t_matrix` initialisée.
 *          Si `n <= 0`, la matrice retournée a `n = 0` et `a = NULL`.
 */
t_matrix mx_zeros(int n)
{
    t_matrix matrix;
    matrix.n = n;
    matrix.a = NULL;

    if (n <= 0) {
        matrix.n = 0;
        return matrix;
    }

    float **rows = (float **)malloc((size_t)n * sizeof(float *));
    if (!rows) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    float *data = (float *)calloc((size_t)n * (size_t)n, sizeof(float));
    if (!data) {
        perror("calloc");
        free(rows);
        exit(EXIT_FAILURE);
    }

    for (int row = 0; row < n; ++row) {
        rows[row] = data + (size_t)row * (size_t)n;
    }

    matrix.a = rows;
    return matrix;
}

/**
 * @brief  Libère la mémoire occupée par une matrice
 *
 * @param[in,out] M  Matrice à libérer (peut être NULL ou vide)
 */
void mx_free(t_matrix *M)
{
    if (!M || !M->a) {
        return;
    }

    free(M->a[0]);
    free(M->a);
    M->a = NULL;
    M->n = 0;
}

/**
 * @brief  Copie le contenu d'une matrice source dans une destination
 *
 * Si `dst` n'est pas initialisée ou si sa taille diffère de `src->n`,
 * la fonction réalloue la matrice destination via `mx_free` puis `mx_zeros`.
 *
 * @param[in]  src  Matrice source à copier
 * @param[out] dst  Matrice destination (peut être non initialisée)
 */
void mx_copy(const t_matrix *src, t_matrix *dst)
{
    if (!src || !dst) {
        return;
    }

    int size = src->n;

    if (size <= 0 || !src->a) {
        mx_free(dst);
        return;
    }

    if (!dst->a || dst->n != size) {
        mx_free(dst);
        *dst = mx_zeros(size);
    }

    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            dst->a[row][col] = src->a[row][col];
        }
    }
}

/**
 * @brief  Construit la matrice de transitions à partir d'une liste d'adjacence
 *
 * La matrice retournée est de taille `n × n` où `n = g->size` et les coefficients sont définis par :
 *   M[i-1][j-1] = proba de l'arête `i -> j` si elle existe, 0 sinon.
 *
 * @param[in] g  Graphe en liste d'adjacence (Markov) déjà initialisé
 *
 * @return  Matrice de transition correspondante
 */
t_matrix mx_from_adjlist(const AdjList *g)
{
    if (!g || g->size <= 0 || !g->array) {
        fprintf(stderr, "[matrix][ERR] Graphe invalide dans mx_from_adjlist\n");
        exit(EXIT_FAILURE);
    }

    int size = g->size;
    t_matrix matrix = mx_zeros(size);

    for (int vertexIndex = 0; vertexIndex < size; ++vertexIndex) {
        List *adjacentList = &g->array[vertexIndex];
        for (Cell *cell = adjacentList->head; cell != NULL; cell = cell->next) {
            int from = vertexIndex + 1;   /* sommets dans le graphe : 1..n */
            int to = cell->dest;          /* destination 1..n */
            if (to < 1 || to > size) {
                fprintf(stderr, "[matrix][ERR] Destination hors bornes %d (1..%d)\n", to, size);
                mx_free(&matrix);
                exit(EXIT_FAILURE);
            }
            matrix.a[from - 1][to - 1] = cell->proba;
        }
    }

    return matrix;
}

/**
 * @brief  Multiplie deux matrices carrées A et B : C = A × B
 *
 * Les trois matrices doivent être de même taille. Si `C` n'est pas
 * correctement dimensionnée, elle est réallouée automatiquement.
 *
 * @param[in]  A  Première matrice (à gauche)
 * @param[in]  B  Deuxième matrice (à droite)
 * @param[out] C  Résultat de la multiplication
 */
void mx_mul(const t_matrix *A, const t_matrix *B, t_matrix *C)
{
    if (!A || !B || !C || !A->a || !B->a || A->n != B->n) {
        fprintf(stderr, "[matrix][ERR] Matrices invalides ou tailles incompatibles dans mx_mul\n");
        exit(EXIT_FAILURE);
    }

    int size = A->n;

    if (!C->a || C->n != size) {
        mx_free(C);
        *C = mx_zeros(size);
    }

    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            float sum = 0.0f;
            for (int kIndex = 0; kIndex < size; ++kIndex) {
                sum += A->a[row][kIndex] * B->a[kIndex][col];
            }
            C->a[row][col] = sum;
        }
    }
}

/**
 * @brief  Somme des valeurs absolues des différences entre deux matrices
 *
 * @param[in] M  Première matrice
 * @param[in] N  Deuxième matrice
 *
 * @return  Somme des valeurs absolues des différences
 */
float mx_diff_abs1(const t_matrix *M, const t_matrix *N)
{
    if (!M || !N || !M->a || !N->a || M->n != N->n) {
        fprintf(stderr, "[matrix][ERR] Matrices invalides ou tailles incompatibles dans mx_diff_abs1\n");
        exit(EXIT_FAILURE);
    }

    int size = M->n;
    float totalDifference = 0.0f;

    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            float delta = M->a[row][col] - N->a[row][col];
            totalDifference += fabsf(delta);
        }
    }

    return totalDifference;
}

/**
 * @brief  Effectue une étape de distribution : pi1 = pi0 × M
 *
 * Les vecteurs `pi0` et `pi1` sont de taille `M->n`
 *
 * @param[in]  pi0  Distribution initiale (taille n)
 * @param[in]  M    Matrice de transition n×n (lignes = états de départ)
 * @param[out] pi1  Distribution résultante (taille n)
 */
void dist_step(const float *pi0, const t_matrix *M, float *pi1)
{
    if (!M || !M->a || M->n <= 0 || !pi0 || !pi1) {
        fprintf(stderr, "[matrix][ERR] Paramètres invalides dans dist_step\n");
        exit(EXIT_FAILURE);
    }

    int size = M->n;

    for (int col = 0; col < size; ++col) {
        float sum = 0.0f;
        for (int row = 0; row < size; ++row) {
            sum += pi0[row] * M->a[row][col];
        }
        pi1[col] = sum;
    }
}

/**
 * @brief  Calcule Pi_t = Pi_0 M^t par itérations successives
 *
 * Utilise `dist_step` de manière répétée pour appliquer `t` transitions.
 * Si `t == 0`, la fonction recopie simplement `pi_0` dans `pi_t`.
 *
 * @param[in]  pi0  Distribution initiale (taille n)
 * @param[in]  M    Matrice de transition n×n
 * @param[in]  t    Nombre d'étapes (t ≥ 0)
 * @param[out] pit  Distribution après t étapes (taille n)
 */
void dist_power(const float *pi0, const t_matrix *M, int t, float *pit)
{
    if (!M || !M->a || M->n <= 0 || !pi0 || !pit || t < 0) {
        fprintf(stderr, "[matrix][ERR] Paramètres invalides dans dist_power\n");
        exit(EXIT_FAILURE);
    }

    int size = M->n;

    /* Cas trivial : t == 0 => copie directe */
    if (t == 0) {
        for (int index = 0; index < size; ++index) {
            pit[index] = pi0[index];
        }
        return;
    }

    float *current = (float *)malloc((size_t)size * sizeof(float));
    float *next = (float *)malloc((size_t)size * sizeof(float));
    if (!current || !next) {
        perror("malloc");
        free(current);
        free(next);
        exit(EXIT_FAILURE);
    }

    for (int index = 0; index < size; ++index) {
        current[index] = pi0[index];
    }

    for (int step = 0; step < t; ++step) {
        dist_step(current, M, next);

        float *tmp = current;
        current = next;
        next = tmp;
    }

    for (int index = 0; index < size; ++index) {
        pit[index] = current[index];
    }

    free(current);
    free(next);
}

/**
 * @brief  Affiche une matrice n×n sur la sortie standard
 *
 * Chaque ligne de la matrice est affichée sur une ligne distincte,
 * avec des coefficients formatés en flottants.
 *
 * @param[in] M  Matrice à afficher
 */
void mx_print(const t_matrix *M)
{
    if (!M || !M->a || M->n <= 0) {
        printf("[empty matrix]\n");
        return;
    }

    int size = M->n;
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            printf("%8.4f", (double)M->a[row][col]);
        }
        printf("\n");
    }
}

//Part 3.3

/**
 * @brief Extrait la sous-matrice correspondant à une composante fortement connexe.
 *
 * @param matrix      Matrice globale de transition (taille N x N).
 * @param part        Partition du graphe en composantes fortement connexes.
 * @param compo_index Indice de la composante dans part (0 .. part.count-1).
 *
 * @return t_matrix   Sous-matrice correspondant à la composante compo_index.
 *                    Si compo_index est invalide ou la classe est vide :
 *                    retourne une matrice de taille 0 (n = 0, a = NULL).
 */
t_matrix subMatrix(t_matrix matrix, Partition part, int compo_index)
{
    t_matrix sub;
    sub.n = 0;
    sub.a = NULL;

    // Vérif de base sur l'indice de composante
    if (compo_index < 0 || compo_index >= part.count) {
        return sub;
    }

    // Récupération de la classe SCC correspondante
    SccClass *cls = &part.classes[compo_index];
    int m = cls->count;  // nombre de sommets dans la composante

    if (m <= 0) {
        // Classe vide -> sous-matrice vide
        return sub;
    }

    // Création d'une matrice m x m initialisée à 0
    sub = mx_zeros(m);

    // Remplissage : projection des indices globaux vers la sous-matrice
    //
    // cls->verts[k] contient des ids de sommets 1..N
    // Les indices de la matrice globale matrix.a sont 0..N-1
    for (int i = 0; i < m; ++i) {
        int gi = cls->verts[i] - 1;  // indice global (ligne)
        if (gi < 0 || gi >= matrix.n) {
            continue;
        }

        for (int j = 0; j < m; ++j) {
            int gj = cls->verts[j] - 1;  // indice global (colonne)
            if (gj < 0 || gj >= matrix.n) {
                continue;
            }

            sub.a[i][j] = matrix.a[gi][gj];
        }
    }

    return sub;
}