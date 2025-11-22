#include <stdlib.h>
#include <stdio.h>
#include "hasse.h"
#include "graph.h"
#include "scc.h"

/**
 * @brief Initialise la structure HasseLinkArray à vide.
 * * @param arr Pointeur vers le tableau de liens de Hasse (HasseLinkArray).
 */
void hasse_init_links(HasseLinkArray *arr){
    arr->links = NULL;
    arr->count = 0; // Nombre de liens actuellement stockés
    arr->capacity = 0; // Capacité totale allouée
}

/**
 * @brief Libère la mémoire allouée pour le tableau de liens de Hasse.
 * * @param arr Pointeur vers le tableau à libérer.
 */
void hasse_free_links(HasseLinkArray *arr){
    free(arr->links);
    arr->capacity = 0;
    arr->count = 0;
}

/**
 * @brief Vérifie si un lien direct entre deux classes existe déjà.
 * * @param arr Pointeur vers le tableau de liens existants.
 * @param c_from Classe de départ (indices 1-basés)
 * @param c_to Classe d'arrivée (indices 1-basés)
 * @return int 1 si le lien existe, 0 sinon.
 */
int hasse_link_exists(const HasseLinkArray *arr, int c_from, int c_to){
    // Parcourt tous les liens existants
    for (int i = 0; i<arr->count; i++) {
        // Les classes sont stockées en interne de manière 0-basée (c_from-1, c_to-1)
        if(arr->links[i].from_class == c_from-1 && arr->links[i].to_class == c_to-1){
            return 1; // Lien trouvé
        }
    }
    return 0; // Lien non trouvé
}

/**
 * @brief Construit une table de correspondance pour trouver rapidement la classe
 * d'appartenance de n'importe quel sommet.
 * * @param p Partition contenant les classes de composantes fortement connexes (SCC).
 * @param n_vertices Nombre total de sommets dans le graphe original.
 * @param class_of_vertex Tableau de sortie (taille n_vertices + 1) où l'indice est le sommet
 * et la valeur est l'indice de sa classe (0-basé).
 */
void build_vertex_to_class_map(Partition *p, int n_vertices, int *class_of_vertex){
    // Parcourt chaque classe (i est l'indice de la classe, 0-basé)
    for(int i =0; i<p->count;i++){
        SccClass v = p->classes[i]; // La classe actuelle
        
        // Parcourt chaque sommet 'k' dans cette classe
        for (int j = 0; j<v.count; j++) {
            int k = v.verts[i]; // Erreur potentielle : devrait être v.verts[j] si 'j' est l'indice du sommet dans la classe 'v'.
            // Correction supposée : int k = v.verts[j];
            
            // Vérifie que l'indice du sommet est valide
            if (k >= 1 && k <= n_vertices) {
                // Stocke l'indice de la classe (i) à la position du sommet (k)
                class_of_vertex[k] = i; // k (sommet 1-basé) -> i (classe 0-basée)
            }
        }
    }
}

/**
 * @brief Détecte les liens directs entre les différentes classes d'équivalence
 * (Composantes Fortement Connexes) à partir du graphe original.
 * * @param g Pointeur vers le graphe d'adjacence original.
 * @param p Pointeur vers la partition des classes (SCC).
 * @param out_links Pointeur vers le tableau de liens de Hasse à construire (croissance dynamique).
 */
void build_class_links(const AdjList *g, const Partition *p, HasseLinkArray *out_links) {
    // Allocation du tableau pour la table de correspondance (n_vertices + 1 pour l'indexation 1-basée)
    int *class_of_vertex = malloc((g->size + 1) * sizeof(int));
    // Remplissage de la table de correspondance
    build_vertex_to_class_map((Partition *)p, g->size, class_of_vertex);

    // 1. Parcourt tous les sommets du graphe original (v est 1-basé)
    for (int v = 1; v <= g->size; v++) {
        // 2. Parcourt toutes les arêtes sortantes du sommet v (v-1 pour tableau 0-basé)
        for (Cell *adj = g->array[v - 1].head; adj != NULL; adj = adj->next) {
            
            // Détermine la classe de départ et d'arrivée
            int from_class = class_of_vertex[v];
            int to_class = class_of_vertex[adj->dest];
            
            // 3. Vérifie si le lien est inter-classes et n'existe pas déjà
            if (from_class != to_class && !hasse_link_exists(out_links, from_class + 1, to_class + 1)) {
                
                // Gestion de la croissance dynamique du tableau de liens (doublage de la capacité)
                if (out_links->count == out_links->capacity) {
                    out_links->capacity = out_links->capacity == 0 ? 4 : out_links->capacity * 2;
                    out_links->links = realloc(out_links->links, out_links->capacity * sizeof(HasseLink));
                }
                
                // 4. Ajout du nouveau lien
                out_links->links[out_links->count].from_class = from_class; // 0-basé
                out_links->links[out_links->count].to_class = to_class;     // 0-basé
                out_links->count++;
            }
        }
    }

    free(class_of_vertex); // Libère la table de correspondance temporaire
}


/**
 * @brief Supprime les liens redondants pour obtenir le diagramme de Hasse
 * (les liens transitifs : A->C si A->B et B->C existent).
 * * @param links Pointeur vers le tableau de liens entre classes à modifier.
 * @param nb_classes Nombre total de classes.
 */
void remove_transitive_links(HasseLinkArray *links, int nb_classes){
    // 1. Allocation d'une matrice d'accessibilité (matrice booléenne carrée de taille nb_classes x nb_classes)
    int **reach = malloc(nb_classes * sizeof(int *));
    for (int i = 0; i < nb_classes; i++) {
        reach[i] = calloc(nb_classes, sizeof(int)); // calloc initialise à 0
    }

    // 2. Remplissage initial de la matrice d'accessibilité avec les liens directs
    for (int i = 0; i < links->count; i++) {
        reach[links->links[i].from_class][links->links[i].to_class] = 1;
    }

    // 3. Calcul de la fermeture transitive (Algorithme de Floyd-Warshall)
    // Après cette étape, reach[i][j] = 1 si la classe i peut atteindre la classe j (directement ou via des intermédiaires).
    for (int k = 0; k < nb_classes; k++) { // Classe intermédiaire
        for (int i = 0; i < nb_classes; i++) { // Classe de départ
            for (int j = 0; j < nb_classes; j++) { // Classe d'arrivée
                // Si i peut atteindre k, et k peut atteindre j, alors i peut atteindre j.
                if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
            }
        }
    }

    // 4. Filtrage : on ne garde que les liens qui ne sont pas transitifs
    int write_idx = 0; // Index pour écrire dans le tableau compacté
    for (int i = 0; i < links->count; i++) {
        int from = links->links[i].from_class;
        int to = links->links[i].to_class;
        int transitive = 0;
        
        // Recherche d'un chemin intermédiaire k tel que from -> k -> to
        for (int k = 0; k < nb_classes; k++) {
            // k doit être différent de from et to
            if (k != from && k != to && reach[from][k] && reach[k][to]) {
                transitive = 1; // Lien transitif trouvé (A->B->C, donc A->C est redondant)
                break;
            }
        }
        
        // Si le lien n'est pas transitif (il fait partie du graphe minimal)
        if (!transitive) {
            links->links[write_idx++] = links->links[i]; // Garde le lien
        }
    }
    // Mise à jour du nombre de liens après le compactage
    links->count = write_idx;

    // 5. Libération de la matrice d'accessibilité
    for (int i = 0; i < nb_classes; i++) free(reach[i]);
    free(reach);
}