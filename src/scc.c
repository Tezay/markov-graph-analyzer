#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scc.h"

/**
 * @brief  Réalloue un bloc mémoire avec vérification stricte
 *
 * Enveloppe de `realloc` qui termine le programme en cas d'échec
 * d'allocation (si `sz != 0`).
 *
 * @param[in]  p   Pointeur existant (ou NULL) à réallouer
 * @param[in]  sz  Nouvelle taille en octets
 *
 * @return  Pointeur vers la zone réallouée. Si `sz == 0`, peut
 *          retourner NULL et libérer éventuellement `p` (selon implémentation).
 *
 * @warning Termine le programme via `exit(EXIT_FAILURE)` si l'allocation échoue.
 */
static void *xrealloc(void *p, size_t sz) {
    void *q = realloc(p, sz);
    if (!q && sz != 0) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    return q;
}

/**
 * @brief  Crée une classe de CFC vide
 *
 * Initialise une structure `SccClass` avec des champs nuls et un
 * compteur à 0.
 *
 * @return  Une structure `SccClass` initialisée à vide.
 */
SccClass scc_make_empty_class(void) {
    SccClass c;
    c.verts = NULL;
    c.count = 0;
    c.capacity = 0;
    return c;
}

/**
 * @brief  Ajoute un sommet à une classe de CFC
 *
 * Redimensionne automatiquement le tableau interne si nécessaire (capacité doublée).
 *
 * @param[in,out] c  Pointeur vers la classe à enrichir
 * @param[in]     v  Identifiant du sommet (1..N)
 *
 * @pre  `c` non NULL
 */
void scc_add_vertex(SccClass *c, int v) {
    if (!c) return;
    if (c->count >= c->capacity) {
        int newcap = c->capacity > 0 ? c->capacity * 2 : 4;
        c->verts = (int*)xrealloc(c->verts, (size_t)newcap * sizeof(int));
        c->capacity = newcap;
    }
    c->verts[c->count++] = v;
}

/**
 * @brief  Initialise une partition vide
 *
 * Met à zéro les champs de la structure pour représenter une
 * partition sans classes.
 *
 * @param[out] p  Partition à initialiser
 *
 * @pre  `p` non NULL
 * @post `p->classes == NULL`, `p->count == 0`, `p->capacity == 0`
 */
void scc_init_partition(Partition *p) {
    if (!p) return;
    p->classes = NULL;
    p->count = 0;
    p->capacity = 0;
}

/**
 * @brief  Ajoute une classe à la partition
 *
 * Copie superficielle de la classe dans le tableau de la partition,
 * avec réallocation automatique si nécessaire.
 *
 * @param[in,out] p  Partition cible
 * @param[in]     c  Classe à ajouter (copie superficielle)
 *
 * @pre  `p` non NULL
 * @note La propriété de `c.verts` est transférée à la partition; ne pas le libérer ensuite hors de la partition.
 */
void scc_add_class(Partition *p, SccClass c) {
    if (!p) return;
    if (p->count >= p->capacity) {
        int newcap = p->capacity > 0 ? p->capacity * 2 : 4;
        p->classes = (SccClass*)xrealloc(p->classes, (size_t)newcap * sizeof(SccClass));
        p->capacity = newcap;
    }
    p->classes[p->count++] = c; // shallow copy (verts owned by class)
}

/**
 * @brief  Libère une partition et ses classes
 *
 * Libère chaque tableau de sommets des classes, puis le tableau des
 * classes lui‑même, et remet la structure à zéro.
 *
 * @param[in,out] p  Partition à libérer et remettre à zéro
 *
 * @pre  `p` non NULL
 * @post `p->classes == NULL`, `p->count == 0`, `p->capacity == 0`
 */
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
