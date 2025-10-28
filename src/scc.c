#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scc.h"

static void *xrealloc(void *p, size_t sz) {
    void *q = realloc(p, sz);
    if (!q && sz != 0) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    return q;
}

SccClass scc_make_empty_class(void) {
    SccClass c;
    c.verts = NULL;
    c.count = 0;
    c.capacity = 0;
    return c;
}

void scc_add_vertex(SccClass *c, int v) {
    if (!c) return;
    if (c->count >= c->capacity) {
        int newcap = c->capacity > 0 ? c->capacity * 2 : 4;
        c->verts = (int*)xrealloc(c->verts, (size_t)newcap * sizeof(int));
        c->capacity = newcap;
    }
    c->verts[c->count++] = v;
}

void scc_init_partition(Partition *p) {
    if (!p) return;
    p->classes = NULL;
    p->count = 0;
    p->capacity = 0;
}

void scc_add_class(Partition *p, SccClass c) {
    if (!p) return;
    if (p->count >= p->capacity) {
        int newcap = p->capacity > 0 ? p->capacity * 2 : 4;
        p->classes = (SccClass*)xrealloc(p->classes, (size_t)newcap * sizeof(SccClass));
        p->capacity = newcap;
    }
    p->classes[p->count++] = c; // shallow copy (verts owned by class)
}

void scc_free_partition(Partition *p) {
    if (!p) return;
    if (p->classes) {
        for (int i = 0; i < p->count; ++i) {
            free(p->classes[i].verts);
            p->classes[i].verts = NULL;
            p->classes[i].count = 0;
            p->classes[i].capacity = 0;
        }
        free(p->classes);
    }
    p->classes = NULL;
    p->count = 0;
    p->capacity = 0;
}

