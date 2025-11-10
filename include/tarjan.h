#ifndef TARJAN_H
#define TARJAN_H
#include "graph.h"
#include "scc.h"

// Lance Tarjan sur g et remplit 'out' avec la partition (ordre des classes indifférent)
void tarjan_partition(const AdjList *g, Partition *out);

#endif