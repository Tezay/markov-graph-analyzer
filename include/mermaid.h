#ifndef MERMAID_H
#define MERMAID_H
#include "graph.h"

// Exporte le graphe au format Mermaid
int export_mermaid(const AdjList *g, const char *outfile);

#endif