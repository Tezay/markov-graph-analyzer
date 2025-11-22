#include "graph.h" // Inclut les définitions de AdjList, graph_init, graph_add_edge, graph_print et graph_free.

/**
 * @brief Programme de démonstration pour l'utilisation des fonctions de gestion de graphe.
 */
int main(void) {
    AdjList g;
    // 1. Initialisation du graphe pour 4 sommets (états). Les sommets seront numérotés 1 à 4.
    graph_init(&g, 4);

    // 2. Ajout des arêtes (transitions) avec leurs probabilités :

    // A partir du sommet 1 :
    graph_add_edge(&g, 1, 2, 0.5f); // Transition 1 -> 2 avec probabilité 0.5
    graph_add_edge(&g, 1, 3, 0.5f); // Transition 1 -> 3 avec probabilité 0.5
    // Note : 0.5 + 0.5 = 1.0. La somme des probabilités sortantes de l'état 1 est bien 1.

    // A partir du sommet 2 :
    graph_add_edge(&g, 2, 4, 1.0f); // Transition 2 -> 4 avec probabilité 1.0

    // A partir du sommet 3 :
    graph_add_edge(&g, 3, 4, 1.0f); // Transition 3 -> 4 avec probabilité 1.0
    
    // 3. Affichage du graphe (devrait montrer les listes d'adjacence des 4 sommets).
    graph_print(&g);

    // 4. Libération de la mémoire allouée par le graphe.
    graph_free(&g);
    return 0;
}