#ifndef MERMAID_HASSE_H
#define MERMAID_HASSE_H
#include "scc.h"
#include "hasse.h"

// Exporte le Hasse (noeuds = C1..Ck avec liste des membres)
int export_hasse_mermaid(const Partition *p, const HasseLinkArray *links, const char *outfile);

// Exporte la partition seule (texte) si utile
int export_partition_text(const Partition *p, const char *outfile);

#endif