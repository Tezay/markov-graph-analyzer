#include <stdio.h>    // Fonctions d'entrée/sortie (printf)
#include <stdlib.h>   // Fonctions utilitaires (EXIT_FAILURE)
#include "graph.h"    // Définitions et fonctions pour le graphe d'adjacence (AdjList)
#include "scc.h"      // Définitions pour la Partition (Partition, SccClass)
#include "tarjan.h"   // Fonction d'identification des SCC (tarjan_partition)
#include "hasse.h"    // Fonctions pour le graphe de condensation (HasseLinkArray, build_class_links)

/**
 * @brief Affiche le contenu de la partition (la liste des Composantes Fortement Connexes ou SCCs).
 *
 * @param p Pointeur vers la structure Partition.
 */
static void print_partition(const Partition *p) {
    printf("=== Partition (CFC) ===\n");
    // Itère sur chaque classe
    for (int i = 0; i < p->count; i++) {
        printf("Classe %d: ", i);
        // Itère sur les sommets contenus dans la classe actuelle
        for (int j = 0; j < p->classes[i].count; j++) {
            printf("%d ", p->classes[i].verts[j]); // Affiche l'indice du sommet (probablement 1-basé)
        }
        printf("\n");
    }
}

/**
 * @brief Affiche les liens inter-classes (les arêtes du graphe de condensation ou de Hasse).
 *
 * @param arr Pointeur vers le tableau des liens de Hasse.
 */
static void print_links(const HasseLinkArray *arr) {
    printf("=== Liens entre classes ===\n");
    // Itère sur chaque lien enregistré
    for (int i = 0; i < arr->count; i++) {
        // Affiche la transition : Classe de départ -> Classe d'arrivée
        printf("Classe %d -> Classe %d\n", arr->links[i].from_class, arr->links[i].to_class);
    }
}

/**
 * @brief Fonction principale de démonstration pour la décomposition en classes (SCC)
 * et la construction du graphe de Hasse.
 */
int main(void) {
    printf("=== TEST Partie 2.2 : hasse_links (liens inter-classes) ===\n");
    AdjList g;
    graph_init(&g, 6); // Initialise un graphe avec 6 sommets

    // Définition d'un graphe d'exemple pour le test
    graph_add_edge(&g, 1, 2, 1.0f);
    graph_add_edge(&g, 2, 3, 1.0f);
    graph_add_edge(&g, 3, 1, 1.0f);  // Composante Fortement Connexe (SCC) {1, 2, 3} (Classe A)

    graph_add_edge(&g, 4, 5, 1.0f);
    graph_add_edge(&g, 5, 4, 1.0f);  // SCC {4, 5} (Classe B)
    
    graph_add_edge(&g, 3, 4, 1.0f);  // Lien A -> B (3 est dans A, 4 est dans B)
    graph_add_edge(&g, 5, 6, 1.0f);  // Lien B -> {6}
    
    // Le sommet 6 est une SCC seule (Classe C)

    printf("=== Graphe ===\n");
    graph_print(&g); // Affiche le graphe d'adjacence

    Partition p;
    scc_init_partition(&p); // Initialise la structure qui va contenir les classes
    
    // Application de l'algorithme de Tarjan pour trouver les SCCs
    tarjan_partition(&g, &p);
    print_partition(&p); // Affiche le résultat de la partition

    HasseLinkArray links = {NULL, 0, 0}; // Initialisation manuelle des liens
    
    // Construction des liens entre les classes (graphe de condensation)
    build_class_links(&g, &p, &links);
    print_links(&links); // Affiche les liens trouvés (e.g., Classe A -> Classe B)

    // --- Libération de la mémoire ---
    hasse_free_links(&links); // Libère les liens de Hasse
    scc_free_partition(&p);   // Libère la partition des SCCs
    graph_free(&g);           // Libère le graphe d'adjacence
    printf("=== FIN TEST hasse_links ===\n");
    return 0;
}
