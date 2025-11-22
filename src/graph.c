#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "list.h" // Nécessaire pour les fonctions list_init, list_free, list_push_front, et list_print

/**
 * @brief        Fonction qui initialise un graph de taille n
 *
 * @param g      Pointeur vers le graphe g (structure AdjList)
 * @param n      Taille de g (nombre de sommets)
 */
void graph_init(AdjList *g, int n){
    // 1. Stocke le nombre de sommets dans la structure du graphe
    g->size = n;
    
    // 2. Alloue dynamiquement un tableau de 'n' listes (une liste par sommet)
    g->array = malloc(n * sizeof(List));

    //* Si erreur d'allocation
    if (!g->array) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    // 3. Initialise chaque élément du tableau (chaque liste d'adjacence)
    for (int i = 0; i < n; i++) {
        list_init(&g->array[i]); // Utilise la fonction d'initialisation de list.c
    }
}

/**
 * @brief        Libère un graph g
 * * @param g      Graph qui va être libéré (Pointeur vers AdjList)
 */
void graph_free(AdjList *g){
    
    // 1. Libère la mémoire de toutes les listes d'adjacence (les arêtes)
    for (int i = 0; i < g->size; i++) {
        list_free(&g->array[i]); // Utilise la fonction de libération de list.c
    }
    
    // 2. Libère la mémoire du tableau lui-même (qui contenait les pointeurs vers les listes)
    free(g->array);
    // Note: Il serait bon de mettre g->array = NULL après le free pour éviter les pointeurs pendus.
}

/**
 * @brief       Ajoute une arête orientée dans un graphe de Markov
 * * @param g     Pointeur vers le graphe à modifier
 * @param from  Sommet de départ de l'arête (doit être entre 1 et g->size)
 * @param to    Sommet d'arrivée de l'arête (doit être entre 1 et g->size)
 * @param proba Probabilité de transition associée à l'arête
 */
void graph_add_edge(AdjList *g, int from, int to, float proba) {
    // La vérification des indices est essentielle pour la sécurité. Les sommets sont 1-basés
    // dans l'interface, mais 0-basés dans le tableau g->array.
    if ((from < 1 || from > g->size) || (to < 1 || to > g->size)) {
        fprintf(stderr, "indices invalides\n");
        exit(EXIT_FAILURE);
    }
    
    // Ajout de l'arête dans la liste d'adjacence du sommet de départ 'from'.
    // On utilise 'from - 1' car le tableau est 0-basé.
    // L'arête est ajoutée au début (list_push_front).
    list_push_front(&g->array[from - 1], to, proba);
}

/**
 * @brief  Affiche le graphe sous forme de listes d'adjacence
 *
 * @param g  Pointeur vers le graphe à afficher
 */
void graph_print(const AdjList *g) {
    // Parcourt tous les sommets de 0 à size - 1
    for (int i = 0; i < g->size; i++) {
        // Affiche l'indice du sommet (i + 1 pour l'affichage 1-basé)
        printf("Sommet %d: ", i + 1);
        
        // Affiche la liste d'adjacence (transitions sortantes) pour ce sommet
        list_print(g->array[i]);
    }
}