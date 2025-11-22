#ifndef MATRIX_H
#define MATRIX_H
#include "graph.h"
#include "scc.h"

typedef struct {
    int n;          // taille n x n
    float **a;      // a[i][j]
} t_matrix;

t_matrix mx_from_adjlist(const AdjList *g);
t_matrix mx_zeros(int n);
void     mx_copy(const t_matrix *src, t_matrix *dst);
void     mx_mul(const t_matrix *A, const t_matrix *B, t_matrix *C);
float    mx_diff_abs1(const t_matrix *M, const t_matrix *N);
void     mx_free(t_matrix *M);

void dist_step(const float *pi0, const t_matrix *M, float *pi1);
void dist_power(const float *pi0, const t_matrix *M, int t, float *pit);
void mx_print(const t_matrix *M);

t_matrix subMatrix(t_matrix matrix, Partition part, int compo_index);

int mx_power_until_diff(const t_matrix *M, float eps, int max_iter, t_matrix *out, int *iters_done);
int stationary_distribution(const t_matrix *MC, float eps, int max_iter, float *pi_out);

#endif
