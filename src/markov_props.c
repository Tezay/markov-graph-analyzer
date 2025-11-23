#include <stdio.h>
#include <stdlib.h>
#include "markov_props.h"
#include "utils.h"


/**
 * @brief  Trouve l'index de la classe contenant un sommet donné
 *
 * Parcourt toutes les classes de la partition et recherche `vertex`
 * dans chacun des tableaux `verts[]`.
 *
 * @param[in]  part    Partition de SCC (peut être NULL)
 * @param[in]  vertex  Identifiant du sommet recherché
 *
 * @return  Index de la classe (0..part->count-1) si trouvé, sinon -1
 */
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

/**
 * @brief  Indique si une classe possède au moins un lien sortant vers une autre classe
 *
 * Vérifie l'existence d'au moins un lien `from_class == class_idx` avec
 * `to_class != class_idx`.
 *
 * @param[in]  links      Tableau des liens du Hasse
 * @param[in]  class_idx  Index de la classe à tester
 *
 * @return  1 si au moins un lien sortant existe, sinon 0
 *
 */
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

/**
 * @brief  Étiquette chaque classe comme transitoire ou persistante
 *
 * Remplit deux tableaux binaires (0/1) indiquant, pour chaque classe,
 * si elle est transitoire ou persistante.
 *
 * Convention :
 * - transitoire = 1 si la classe a AU MOINS un lien sortant,
 * - persistante = 1 si la classe N'A AUCUN lien sortant.
 *
 * @param[in]   links          Tableau des liens entre classes (peut être NULL)
 * @param[in]   nb_classes     Nombre total de classes
 * @param[out]  is_transient   Tableau (taille nb_classes) rempli à 1 si transitoire, 0 sinon (si non NULL)
 * @param[out]  is_persistent  Tableau (taille nb_classes) rempli à 1 si persistante, 0 sinon (si non NULL)
 */
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

/**
 * @brief  Indique si le graphe est irréductible
 *
 * Avec la partition en composantes fortement connexes (SCC), le graphe
 * est irréductible s'il n'existe qu'une seule classe.
 *
 * @param[in]  part  Partition en SCC
 *
 * @return  1 si `part->count == 1`, sinon 0
 */
int markov_is_irreducible(const Partition *part) {
  //On vérifie si il y a une partition
    if (!part) {
        return 0;
    }
    //Si c'est > 1 alors plusieur classe donc non irreductible
    return (part->count == 1) ? 1 : 0;
}

/**
 * @brief  Teste si un sommet est absorbant
 *
 * Un sommet `v` est absorbant si :
 *  1) la classe contenant `v` existe,
 *  2) cette classe a exactement 1 sommet,
 *  3) cette classe n'a aucun lien sortant vers une autre classe.
 *
 * @param[in]  part   Partition en SCC
 * @param[in]  links  Tableau des liens du Hasse
 * @param[in]  vertex Sommet à tester
 *
 * @return  1 si `vertex` est absorbant, sinon 0
 */
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