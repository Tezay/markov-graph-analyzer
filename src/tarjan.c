#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tarjan.h"
#include "graph.h"
#include "list.h"

/**
 * @brief  Alloue un bloc mémoire avec vérification stricte
 *
 * Enveloppe de `malloc` qui termine le programme en cas d'échec
 * d'allocation (si `sz != 0`).
 *
 * @param[in]  sz  Taille en octets à allouer
 *
 * @return  Pointeur alloué (non NULL si `sz > 0`). Peut valoir NULL si `sz == 0`.
 *
 * @warning Termine le programme via `exit(EXIT_FAILURE)` en cas d'échec.
 */
static void *xmalloc(size_t sz) {
    void *p = malloc(sz);
    if (!p && sz != 0) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

/**
 * @brief  Empile un sommet dans la pile du contexte
 *
 * Agrandit la pile dynamiquement si nécessaire (capacité doublée).
 *
 * @param[in,out] C  Contexte de l'algorithme (pile modifiée)
 * @param[in]     v  Identifiant du sommet à empiler
 *
 * @return  void
 *
 * @pre  `C` non NULL
 */
static void push(t_ctx *C, int v) {
    if (C->sp >= C->cap) {
        int newcap = C->cap > 0 ? C->cap * 2 : 16;
        int *ns = (int*)realloc(C->stack, (size_t)newcap * sizeof(int));
        if (!ns) { perror("realloc(stack)"); exit(EXIT_FAILURE); }
        C->stack = ns;
        C->cap = newcap;
    }
    C->stack[C->sp++] = v;
}

/**
 * @brief  Dépile un sommet de la pile du contexte
 *
 * Renvoie -1 si la pile est vide (cas qui ne devrait pas survenir
 * lors d'un déroulement correct de Tarjan).
 *
 * @param[in,out] C  Contexte de l'algorithme (pile modifiée)
 *
 * @return  Identifiant du sommet dépilé, ou -1 si vide
 */
static int pop(t_ctx *C) {
    if (C->sp <= 0) return -1; // devrait pas arriver
    return C->stack[--C->sp];
}

/**
 * @brief  Étape récursive principale de l'algorithme de Tarjan
 *
 * Marque le sommet `v_id`, explore ses successeurs et met à jour les
 * valeurs `index`/`lowlink`. Si `v_id` est racine d'une composante fortement
 * connexe, dépile la composante et l'ajoute à la partition de sortie.
 *
 * @param[in,out] C     Contexte global de Tarjan (modifié)
 * @param[in]     v_id  Identifiant du sommet courant (1..N)
 *
 * @return  void
 *
 * @pre  `C` initialisé, `C->V[v_id].index == -1` au premier appel
 */
static void strongconnect(t_ctx *C, int v_id) {
    t_tarjan_vertex *V = C->V;
    t_tarjan_vertex *v = &V[v_id];

    v->index = C->next_index;
    v->lowlink = C->next_index;
    C->next_index++;

    push(C, v_id);
    v->on_stack = 1;

    // Pour chaque successeur w de v
    for (Cell *c = C->g->array[v_id - 1].head; c; c = c->next) {
        int w_id = c->dest;
        if (V[w_id].index == -1) {
            strongconnect(C, w_id);
            if (V[w_id].lowlink < v->lowlink) v->lowlink = V[w_id].lowlink;
        } else if (V[w_id].on_stack) {
            if (V[w_id].index < v->lowlink) v->lowlink = V[w_id].index;
        }
    }

    // Si v est racine d'une composante fortement connexe
    if (v->lowlink == v->index) {
        SccClass cls = scc_make_empty_class();
        while (1) {
            int w = pop(C);
            if (w < 0) break; // sécurité
            C->V[w].on_stack = 0;
            scc_add_vertex(&cls, w);
            if (w == v_id) break;
        }
        scc_add_class(C->out, cls);
    }
}

/**
 * @brief  Calcule les CFC d'un graphe par l'algorithme de Tarjan
 *
 * Parcourt tous les sommets du graphe et applique Tarjan pour produire
 * une partition en composantes fortement connexes.
 *
 * @param[in]  g    Graphe d'entrée (liste d'adjacence), `g->size >= 1`
 * @param[out] out  Partition résultat, initialisée via `scc_init_partition`
 *
 * @return  void
 *
 * @pre   `g` valide; `out` initialisé par `scc_init_partition`
 * @note  Complexité en O(N + M), avec N sommets et M arêtes.
 */
void tarjan_partition(const AdjList *g, Partition *out) {
    if (!g || g->size <= 0) return;
    // Précondition : 'out' a été initialisé avec scc_init_partition avant

    // Initialisation du contexte de l'algo Tarjan à 'g' et 'out'
    t_ctx C;
    C.g = g;
    C.out = out;
    C.N = g->size;
    C.V = (t_tarjan_vertex*)xmalloc((size_t)(C.N + 1) * sizeof(t_tarjan_vertex));
    C.stack = NULL;
    C.sp = 0;
    C.cap = 0;
    C.next_index = 0;

    // Init le tableau de sommet dans le contexte 'C' avec les valeurs de 'g'
    for (int i = 1; i <= C.N; ++i) {
        C.V[i].id = i;
        C.V[i].index = -1;    // non défini
        C.V[i].lowlink = -1;
        C.V[i].on_stack = 0;
    }

    // Parcourt tous les sommets de 'g' (stockés dans le tab 'C.V[]')
    for (int i = 1; i <= C.N; ++i) {
        if (C.V[i].index == -1) {
            // Appel recurcif de la fonction
            strongconnect(&C, i);
        }
    }

    free(C.V);
    free(C.stack);
}
