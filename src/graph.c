#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h"

//**
// * @brief         Fonction qui initialise un graph de taille n
// *
// * @param g       Pointeur vers le graphe g
// * @param n       Taille de g
// */
void graph_init(AdjList *g, int n){
    g->size = n;
    g->array = malloc(n*sizeof(List));

    //* Si erreur d'allocation
    if (!g->array) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        list_init(&g->array[i]);
    }
}

//**
// * @brief         Libère un graph g
// * 
// * @param g       Graph qui va être libéré
 // */
void graph_free(AdjList *g){
    
    for (int i = 0; i < g->size; i++) {
        list_free(&g->array[i]);
    }
    free(g->array);
}

//**
// * @brief       Ajoute une arête orientée dans un graphe de Markov
// * 
// * @param g     Pointeur vers le graphe à modifier
// * @param from  Sommet de départ de l'arête (doit être entre 1 et g->size)
// * @param to    Sommet d'arrivée de l'arête (doit être entre 1 et g->size)
// * @param proba Probabilité de transition associée à l'arête
// */
void graph_add_edge(AdjList *g, int from, int to, float proba) {
    if ((from < 1 || from > g->size) || (to < 1 || to > g->size)) {
        fprintf(stderr, "indices invalides\n");
        exit(EXIT_FAILURE);
    }
    list_push_front(&g->array[from - 1], to, proba);
}

//**
// * @brief  Affiche le graphe sous forme de listes d'adjacence
// *
// * @param g  Pointeur vers le graphe à afficher
// */
void graph_print(const AdjList *g) {
    for (int i = 0; i < g->size; i++) {
        printf("Sommet %d: ", i + 1);
        list_print(g->array[i]);
    }
}
