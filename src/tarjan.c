#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tarjan.h"
#include "graph.h"
#include "list.h"

/**
 * @brief  Alloue un bloc mémoire avec vérification stricte
 *
 * Enveloppe de `malloc` qui termine le programme en cas d'échec d'allocation (si `sz != 0`).
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
 * @pre  `C` non NULL
 */
static void push(t_ctx *C, int v) {
    // Vérif si pile pleine
    if (C->sp >= C->cap) {
        // Si pleine, double la capacité (ou initialise à 16 si 0)
        int newcap = C->cap > 0 ? C->cap * 2 : 16;
        // Réallocation mémoire
        int *ns = (int*)realloc(C->stack, (size_t)newcap * sizeof(int));
        // Vérif échec realloc
        if (!ns) { perror("realloc(stack)"); exit(EXIT_FAILURE); }
        // Mise à jour du contexte
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
 * @pre  `C` initialisé, `C->V[v_id].index == -1` au premier appel
 */
static void strongconnect(t_ctx *C, int v_id) {
    // Récupère le sommet courant
    t_tarjan_vertex *V = C->V;
    t_tarjan_vertex *v = &V[v_id];

    // Initialise index et lowlink de v
    v->index = C->next_index;
    v->lowlink = C->next_index;
    // Incrémente l'index global pour le prochain sommet
    C->next_index++;

    // Empile v sur la pile C
    push(C, v_id);
    v->on_stack = 1;

    // Pour chaque successeur w de v
    for (Cell *c = C->g->array[v_id - 1].head; c; c = c->next) {
        // Récupère l'identifiant du successeur
        int w_id = c->dest;
        // Vérif si w n'a pas encore été visité
        if (V[w_id].index == -1) {
            // Appel récursif sur w
            strongconnect(C, w_id);
            // Mise à jour du lowlink de v
            if (V[w_id].lowlink < v->lowlink) v->lowlink = V[w_id].lowlink;
        // Sinon, si w est sur la pile, mise à jour du lowlink de v
        } else if (V[w_id].on_stack) {
            if (V[w_id].index < v->lowlink) v->lowlink = V[w_id].index;
        }
    }

    // Si v est racine d'une composante fortement connexe
    if (v->lowlink == v->index) {
        // Crée une nouvelle classe SCC
        SccClass cls = scc_make_empty_class();
        // Dépile tous les sommets de la composante (jusqu'à v)
        while (1) {
            int w = pop(C);
            if (w < 0) break; // sécurité
            C->V[w].on_stack = 0;
            scc_add_vertex(&cls, w); // Ajoute w à la classe courante
            if (w == v_id) break;
        }
        // Ajoute la classe complète à la partition de sortie
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
 * @pre   `g` valide; `out` initialisé par `scc_init_partition`
 * @note  Complexité en O(N + M), avec N sommets et M arêtes.
 */
void tarjan_partition(const AdjList *g, Partition *out) {
    if (!g || g->size <= 0) return;

    // Init du contexte de l'algorithme
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
