#include <stdio.h>
#include <stdlib.h>
#include "markov_props.h"
#include "utils.h"


// -----------------------------------------------------------------
//           Fonction :  find_class_of_vertex
//  Rôle : retrouver dans quelle classe (index) se trouve le sommet v
//  Parcourt toutes les classes de la partition et cherche v dans
//  chacun des tableaux verts[].
//  Retour :
//    - index de la classe (0..p->count-1) si trouvé
//    - -1 si le sommet n'est dans aucune classe (cas d'erreur)
// -----------------------------------------------------------------
static int find_class_of_vertex(const Partition *part, int vertex) {
    if (!part) {
        return -1;
    }

    for (int cls_idx = 0; cls_idx < part->count; ++cls_idx) {
        const SccClass *cls = &part->classes[cls_idx];
        for (int j = 0; j < cls->count; ++j) {
            if (cls->verts[j] == vertex) {
                return cls_idx; //Trouvé
            }
        }
    }
    //Pas trouvé
    return -1;
}

// -----------------------------------------------------------------
//  Fonction statique : class_has_outgoing_link
//  Rôle : dire si une classe donnée possède AU MOINS UN lien sortant
//         vers une AUTRE classe (to != from).
//  Retour :
//    - 1 si la classe a au moins un lien sortant
//    - 0 sinon
// -----------------------------------------------------------------
static int class_has_outgoing_link(const HasseLinkArray *links, int class_idx) {
    if (!links) {
        return 0; //Pas de structure
    }

    for (int i = 0; i < links->count; ++i) {
        const HasseLink *hl = &links->links[i];
        if (hl->from_class == class_idx && hl->to_class != class_idx) {
            return 1; //On a trouvé au moins un lien sortant
        }
    }
    return 0;
}



// -----------------------------------------------------------------
//               Fonction : markov_class_types
//  Rôle : remplir deux tableaux 0/1 indiquant pour chaque classe si
//         elle est transitoire ou persistante.
//  Convention utilisée :
//    - transitoire = 1 si la classe a AU MOINS un lien sortant
//    - persistante = 1 si la classe N'A AUCUN lien sortant.
// -----------------------------------------------------------------
void markov_class_types(const HasseLinkArray *links, int nb_classes,
                        int *is_transient, int *is_persistent)
{
    if (nb_classes <= 0) {
        return;
    }

    //Initialiser les deux tableaux
    //Au départ, on suppose tout le monde PERSISTANT
    for (int i = 0; i < nb_classes; ++i) {
        if (is_transient)  is_transient[i]  = 0;
        if (is_persistent) is_persistent[i] = 1;
    }

    //Parcourir tous les liens et marquer les classes "from" comme transitoires
    if (links) {
        for (int i = 0; i < links->count; ++i) {
            int from = links->links[i].from_class;
            int to   = links->links[i].to_class;

            // on ne compte pas les boucles de classe (from == to) et on vérifie que from ne dépasse pas le nb de classe
            if (from >= 0 && from < nb_classes && from != to) {
                if (is_transient)  is_transient[from]  = 1;
                if (is_persistent) is_persistent[from] = 0;
            }
        }
    }
}

// -----------------------------------------------------------------
//              Fonction : markov_is_irreducible
//  Rôle : dire si le graphe est irréductible.
//         Avec la partition en SCC, c'est très simple :
//         s'il n'y a qu'une seule classe, le graphe est irréductible.
//  Retour :
//    - 1 si p->count == 1
//    - 0 sinon
// -----------------------------------------------------------------
int markov_is_irreducible(const Partition *part) {
  //On vérifie si il y a une partition
    if (!part) {
        return 0;
    }
    //Si c'est > 1 alors plusieur classe donc non irreductible
    return (part->count == 1) ? 1 : 0;
}

// -----------------------------------------------------------------
//              Fonction : markov_is_absorbing_vertex
//  Rôle : dire si un sommet v est ABSORBANT.
//  Donc on vérifie :
//    1. retrouver la classe de v
//    2. vérifier que cette classe a exactement 1 élément
//    3. vérifier que cette classe N'A PAS de lien sortant
//  Retour :
//    - 1 si v est absorbant
//    - 0 sinon
// -----------------------------------------------------------------
int markov_is_absorbing_vertex(const Partition *part, const HasseLinkArray *links, int vertex) {
  //Vérification si la partition existe bien
    if (!part) {
        return 0;
    }

    //Trouver dans quelle classe se trouve le vertex
    int cls_idx = find_class_of_vertex(part, vertex);
    if (cls_idx < 0 || cls_idx >= part->count) {
        //Sommet pas trouvé dans la partition
        return 0;
    }

    const SccClass *cls = &part->classes[cls_idx];

    //Vérifier que la classe a un seul vertex, taille 1
    if (cls->count != 1) {
        return 0;
    }

    //Vérifier que la classe n'a pas de lien sortant
    if (class_has_outgoing_link(links, cls_idx)) {
        return 0;
    }

    //Les 3 conditions sont remplies
    return 1;
}