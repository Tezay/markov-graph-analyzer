#ifndef MATRIX_SUB_H
#define MATRIX_SUB_H
#include "matrix.h"
#include "scc.h"

t_matrix subMatrix(const t_matrix *M, const Partition *part, int class_index);

#endif