#ifndef SCC_H
#define SCC_H

typedef struct {
    int *verts;      // ids des sommets (1..N)
    int  count;      // nombre d'éléments
    int  capacity;
} SccClass;

typedef struct {
    SccClass *classes;
    int       count;
    int       capacity;
} Partition;

// utilitaires allocation/libération
void     scc_init_partition(Partition *p);
void     scc_free_partition(Partition *p);
void     scc_add_vertex(SccClass *c, int v);
SccClass scc_make_empty_class(void);
void     scc_add_class(Partition *p, SccClass c);

#endif