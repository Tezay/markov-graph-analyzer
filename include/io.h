#ifndef IO_H
#define IO_H

#include "graph.h"

// Lit un fichier texte et construit le graphe 'out'.
// Format attendu :
//   N
//   from to proba
//   ...
// En cas d'erreur IO/format, affiche un message et exit(EXIT_FAILURE).
void read_graph_from_file(const char *filename, AdjList *out);

#endif