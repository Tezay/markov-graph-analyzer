#ifndef IO_H
#define IO_H
#include "graph.h"

// Lecture d’un fichier et construction du graphe
// Lève exit(EXIT_FAILURE) en cas d’erreur d’IO ou format.
void read_graph_from_file(const char *filename, AdjList *out);

#endif